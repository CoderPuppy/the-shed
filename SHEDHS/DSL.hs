{-# LANGUAGE RankNTypes, GeneralizedNewtypeDeriving, DerivingStrategies, BlockArguments, MultiParamTypeClasses, FlexibleInstances, ScopedTypeVariables, ImportQualifiedPost #-}

module SHEDHS.DSL where

import Prelude hiding (const)

import Control.Arrow
import Control.Monad
import Control.Monad.Fix
import Control.Monad.Trans.Class
import Control.Monad.Trans.Maybe
import Control.Monad.Trans.RWS.Lazy
import Data.Foldable
import Data.Int
import Data.IntMap qualified as IM
import Data.List
import Data.List.Index (modifyAt, indexed)
import Data.Maybe
import Data.Void
import Data.Word
import Numeric

import Debug.Trace

import SHEDHS.Instruction
import SHEDHS.Assembler

-- TODO: type safe calling conventions
-- TODO: allow pending across jumps
-- TODO: fall-through EBBs
-- TODO: auto delay slots

beltSize :: Num a => a
beltSize = 8

type Error = String
type Prog' = RWS () [Error] [Instr]
type Code' b = MaybeT (RWST () () ([Operand b], [[Operand b]]) Prog')
newtype Code p b a
  = Code { unCode :: Code' b a }
  deriving newtype (Functor, Applicative, Monad)
instance MonadFail (Code p b) where
  fail msg = do
    reportError msg
    Code endOfControl
newtype Prog p a
  = Prog { unProg :: Prog' a }
  deriving newtype (Functor, Applicative, Monad, MonadFix)
data Operand b = Operand {
  opInstr :: Int,
  opDebug :: String,
  opSlot :: Int
} deriving stock (Show, Eq)
data EBB p = EBB {
  ebbName :: String,
  ebbAddr :: Word16
} deriving stock (Show)
type DAddr b = (Operand b, SEImm)

ebb :: String -> (forall b. [Operand b] -> Code p b Void) -> Prog p (EBB p)
ebb name body = Prog do
  instrs <- get
  let addr = fromIntegral $ length instrs
  let args = fmap (Operand (-1) "ARGS") [0..beltSize - 1]
  runRWST (runMaybeT $ unCode $ body args) () (args, repeat [])
  pure $ EBB name addr

prog :: (forall p. Prog p (EBB p)) -> ([Error], ([Instr], Word16))
prog (Prog body) = (errors, (reverse instrs, addr))
  where (EBB _ addr, instrs, errors) = runRWS body () []

operandIndex :: Operand b -> Code p b Int
operandIndex op = do
  (belt, _) <- Code $ lift get
  case elemIndex op belt of
    Just idx -> pure idx
    Nothing -> do
      reportError $ "operand (" <> show op <> ") not on belt (" <> show belt <> ")"
      pure (-2)

reportError :: String -> Code p b ()
reportError = Code . lift . lift . tell . pure

accessOperand :: Operand b -> Code p b Int
accessOperand op = do
  idx <- operandIndex op
  if idx >= beltSize
  then do
    reportError "operand dropped off belt"
    pure (-1)
  else pure idx

finishPending :: Code' b ()
finishPending = lift $ modify \(belt, drops:pending) -> (drops <> belt, pending)

addInstr :: Instr -> Code' b ()
addInstr instr = lift $ lift $ modify (instr:)

createResults :: String -> Int -> Int -> Code' b [Operand b]
createResults debug latency n = do
  instrs <- lift $ lift get
  let results = fmap (Operand (length instrs) debug) [0..n - 1]
  if latency == 0
  then lift $ modify $ first (results <>)
  else lift $ modify $ second $ modifyAt (latency - 1) (results <>)
  pure results

basicInstr :: Traversable f => f (Operand b) -> (f Int -> Instr) -> Code p b [Operand b]
basicInstr ops instr = do
  ops <- traverse accessOperand ops
  instr <- pure $ instr ops
  Code do
    results <- createResults (show instr) (latency instr) (numDrops instr)
    addInstr instr
    finishPending
    pure results

assertBelt :: [Maybe (Operand b)] -> Code p b ()
assertBelt expected = do
  (belt, _) <- Code $ lift get
  let mismatches =
        filter (uncurry (/=) . snd) $
        filter (isJust . fst . snd) $
        indexed $ zip expected $ fmap Just belt <> repeat Nothing
  unless (null mismatches) $
    reportError $ "belt mismatch: expected " <> show expected <> ", got " <> show (zipWith (\a b -> b) expected belt)

assertNoPending :: Code p b ()
assertNoPending = do
  (_, pending) <- Code $ lift get
  -- MAGIC NUMBER
  -- really this should be 4, but going with 8 avoids off-by-one errors
  unless (all null $ take 8 pending) $
    reportError $ "pending operations: " <> show (take 8 pending)

relativizeEBB :: EBB p -> Code p b Int8
relativizeEBB (EBB name addr) = do
  instrs <- Code $ lift $ lift get
  let offset :: Int = fromIntegral addr - (length instrs + 1)
  unless (offset >= -128 && offset <= 127) $
    reportError $ "EBB out of range: " <> name
  pure $ fromIntegral offset

endOfControl :: Code' b a
endOfControl = MaybeT $ pure Nothing

-- getBelt :: Code p b [Operand b]
-- getBelt = fmap (take beltSize . fst . snd) $ Code $ lift get

st :: Operand b -> DAddr b -> Code p b ()
st d (a, o) = fmap (\[] -> ()) $ basicInstr [d, a] \[d, a] -> Ist d a o
add :: Operand b -> Operand b -> Code p b (Operand b)
add a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Iadd a b
mul :: Operand b -> Operand b -> Code p b (Operand b, Operand b)
mul a b = fmap (\[c, d] -> (c, d)) $ basicInstr [a, b] \[a, b] -> Imul a b
and :: Operand b -> Operand b -> Code p b (Operand b)
and a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Iand a b
or :: Operand b -> Operand b -> Code p b (Operand b)
or a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Ior a b
xor :: Operand b -> Operand b -> Code p b (Operand b)
xor a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Ixor a b
sll :: Operand b -> Operand b -> Code p b (Operand b)
sll a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Isll a b
srl :: Operand b -> Operand b -> Code p b (Operand b)
srl a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Isrl a b
sra :: Operand b -> Operand b -> Code p b (Operand b)
sra a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Isra a b
addc :: Operand b -> Operand b -> Code p b (Operand b)
addc a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Iaddc a b
sub :: Operand b -> Operand b -> Code p b (Operand b)
sub a b = fmap (\[c] -> c) $ basicInstr [a, b] \[a, b] -> Isub a b
inv :: Operand b -> Code p b (Operand b)
inv a = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Iinv a
neg :: Operand b -> Code p b (Operand b)
neg a = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Ineg a
lcall :: Operand b -> [Maybe (Operand b)] -> Code p b [Operand b]
lcall f args = do
  f <- accessOperand f
  Code finishPending
  Code finishPending
  assertBelt args
  assertNoPending
  Code do
    results <- createResults (show (Ilcall1 f) <> " " <> show args) 0 beltSize
    addInstr $ Ilcall1 f
    addInstr Icall2
    pure results
ljmp :: Operand b -> [Maybe (Operand b)] -> Code p b Void
ljmp a args = do
  a <- accessOperand a
  Code finishPending
  assertBelt args
  assertNoPending
  Code $ addInstr $ Iljmp a
  Code endOfControl
carry :: Operand b -> Code p b (Operand b)
carry a = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Icarry a
halt :: Code p b Void
halt = Code do
  addInstr Ihalt
  endOfControl
nop :: Code p b ()
nop = fmap (\[] -> ()) $ basicInstr [] \[] -> Inop
pc :: Code p b (Operand b)
pc = fmap (\[a] -> a) $ basicInstr [] \[] -> Ipc
ret :: [Maybe (Operand b)] -> Code p b Void
ret rets = do
  Code finishPending
  Code finishPending
  assertBelt rets
  assertNoPending
  Code do
    addInstr Iret1
    addInstr Iret2
    endOfControl
addi :: Operand b -> SEImm -> Code p b (Operand b)
addi a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Iaddi a i
andi :: Operand b -> SEImm -> Code p b (Operand b)
andi a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Iandi a i
ori :: Operand b -> ZEImm -> Code p b (Operand b)
ori a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Iori a i
xori :: Operand b -> ZEImm -> Code p b (Operand b)
xori a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Ixori a i
slli :: Operand b -> ZEImm -> Code p b (Operand b)
slli a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Islli a i
srli :: Operand b -> ZEImm -> Code p b (Operand b)
srli a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Isrli a i
srai :: Operand b -> ZEImm -> Code p b (Operand b)
srai a i = fmap (\[b] -> b) $ basicInstr [a] \[a] -> Israi a i
bzero :: Operand b -> EBB p -> [Maybe (Operand b)] -> Code p b ()
bzero cond ebb args = do
  -- TODO: auto delay slot
  cond <- accessOperand cond
  offset <- relativizeEBB ebb
  Code do
    addInstr $ Ibzero cond offset
    finishPending
  nop
  assertBelt args
  assertNoPending
bneg :: Operand b -> EBB p -> [Maybe (Operand b)] -> Code p b ()
bneg cond ebb args = do
  -- TODO: auto delay slot
  cond <- accessOperand cond
  offset <- relativizeEBB ebb
  Code do
    addInstr $ Ibneg cond offset
    finishPending
  nop
  assertBelt args
  assertNoPending
boflo :: Operand b -> EBB p -> [Maybe (Operand b)] -> Code p b ()
boflo cond ebb args = do
  -- TODO: auto delay slot
  cond <- accessOperand cond
  offset <- relativizeEBB ebb
  Code do
    addInstr $ Iboflo cond offset
    finishPending
  nop
  assertBelt args
  assertNoPending
bcar :: Operand b -> EBB p -> [Maybe (Operand b)] -> Code p b ()
bcar cond ebb args = do
  -- TODO: auto delay slot
  cond <- accessOperand cond
  offset <- relativizeEBB ebb
  Code do
    addInstr $ Ibcar cond offset
    finishPending
  nop
  assertBelt args
  assertNoPending
sts :: Operand b -> ZEImm -> Code p b ()
sts d a = fmap (\[] -> ()) $ basicInstr [d] \[d] -> Ists d a
ld :: DAddr b -> Code p b (Operand b)
ld (a, o) = fmap (\[d] -> d) $ basicInstr [a] \[a] -> Ild a o
const :: SEImm -> Code p b (Operand b)
const i = fmap (\[a] -> a) $ basicInstr [] \[] -> Iconst i
upper :: ZEImm -> Code p b (Operand b)
upper i = fmap (\[a] -> a) $ basicInstr [] \[] -> Iupper i
call :: EBB p -> [Maybe (Operand b)] -> Code p b [Operand b]
call f args = do
  offset <- relativizeEBB f
  Code finishPending
  Code finishPending
  assertBelt args
  assertNoPending
  Code do
    results <- createResults ("call " <> ebbName f <> " " <> show args) 0 beltSize
    addInstr $ Icall1 offset
    addInstr Icall2
    pure results
jmp :: EBB p -> [Maybe (Operand b)] -> Code p b Void
jmp ebb args = do
  offset <- relativizeEBB ebb
  Code do
    addInstr $ Ijmp offset
    finishPending
  nop
  assertBelt args
  assertNoPending
  Code endOfControl
lds :: ZEImm -> Code p b (Operand b)
lds a = fmap (\[d] -> d) $ basicInstr [] \[] -> Ilds a
alloc :: ZEImm -> Code p b ()
alloc n = fmap (\[] -> ()) $ basicInstr [] \[] -> Ialloc n

canDelaySlot :: Instr -> Bool
canDelaySlot (Ist  _ _ _) = True
canDelaySlot (Iadd   _ _) = True
canDelaySlot (Imul   _ _) = False
canDelaySlot (Iand   _ _) = True
canDelaySlot (Ior    _ _) = True
canDelaySlot (Ixor   _ _) = True
canDelaySlot (Isll   _ _) = True
canDelaySlot (Isrl   _ _) = True
canDelaySlot (Isra   _ _) = True
canDelaySlot (Iaddc  _ _) = True
canDelaySlot (Isub   _ _) = True
canDelaySlot (Iinv     _) = True
canDelaySlot (Ineg     _) = True
canDelaySlot (Ilcall1  _) = error "the hell are you doing"
canDelaySlot (Iljmp    _) = False
canDelaySlot (Icarry   _) = True
canDelaySlot  Ihalt       = True
canDelaySlot  Inop        = True
canDelaySlot  Ipc         = False
canDelaySlot  Iret1       = error "the hell are you doing"
canDelaySlot  Iret2       = False
canDelaySlot  Icall2      = False
canDelaySlot (Iaddi  _ _) = True
canDelaySlot (Imuli  _ _) = False
canDelaySlot (Iandi  _ _) = False
canDelaySlot (Iori   _ _) = False
canDelaySlot (Ixori  _ _) = False
canDelaySlot (Islli  _ _) = False
canDelaySlot (Isrli  _ _) = False
canDelaySlot (Israi  _ _) = False
canDelaySlot (Ibzero _ _) = False
canDelaySlot (Ibneg  _ _) = False
canDelaySlot (Iboflo _ _) = False
canDelaySlot (Ibcar  _ _) = False
canDelaySlot (Ists   _ _) = True
canDelaySlot (Ild    _ _) = False
canDelaySlot (Iconst   _) = True
canDelaySlot (Iupper   _) = True
canDelaySlot (Icall1   _) = error "the hell are you doing"
canDelaySlot (Ijmp     _) = False
canDelaySlot (Ilds     _) = False
canDelaySlot (Ialloc   _) = True

buildProg :: FilePath -> (forall p. Prog p (EBB p)) -> IO ()
buildProg path p = do
  let (errors, (instrs, addr)) = prog p
  for_ errors print
  for_ (indexed instrs) \(addr, instr) ->
    putStrLn $ ($ "") $ showHex addr . showString " " . shows instr
  writeFile path $
    unlines $
    (<> [showHex addr ""]) $
    fmap
      (\(addr, instr) ->
        showHex addr "" <> " 1 " <> showHex (assemble instr) "")
      (indexed instrs)
