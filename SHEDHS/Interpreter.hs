{-# LANGUAGE ScopedTypeVariables, RecordWildCards, NamedFieldPuns #-}

module SHEDHS.Interpreter where

import Data.Int
import Data.Word
import Data.Bits
import Data.Bool
import qualified Data.IntMap as IM

import SHEDHS.Instruction

type Value = Word16
type Addr = Word16
data BeltValue = BeltValue {
  bvValue :: Value,
  bvCarry :: Bool,
  bvOFlow :: Bool
} deriving (Show)
type Belt = [BeltValue]
type Memory = IM.IntMap Value
data Frame = Frame {
  frMem :: Memory,
  frRA :: Addr,
  frLen :: Addr
} deriving (Show)
type Stack = [Frame]

data Status
  = Continue
  | Jump Addr
  | Halt
  deriving (Show)
instance Semigroup Status where
  Continue <> b = b
  a <> Continue = a
  Halt <> Jump _ = Halt
  Jump _ <> Halt = Halt
  Jump a <> Jump b = error $ "BAD: jump to " <> show a <> " and " <> show b

write :: Addr -> Value -> Memory -> Memory
write _ 0 m = m
write addr v m = IM.insert (fromIntegral addr) v m

aluAdd :: Value -> Value -> BeltValue
aluAdd x y = BeltValue {
      bvValue = z,
      bvCarry = x1 && y1 || ((x1 || y1) && not z1),
      bvOFlow = x1 == y1 && z1 /= x1
    }
  where
    z = x + y

    x1 = testBit x 15
    y1 = testBit y 15
    z1 = testBit z 15

interpInstr ::
  Belt -> Memory -> Stack -> Addr -> Instr ->
  ((Memory, Stack), [(Status, [BeltValue])])
interpInstr belt dmem smem pc (Ist b1 b2 imm) =
  ((write addr val dmem, smem), (Continue, []):(Continue, []):[])
  where
    addr = bvValue (belt !! b1) + fromIntegral imm
    val = bvValue $ belt !! b2
interpInstr belt dmem smem pc (Iadd b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where bv = aluAdd (bvValue $ belt !! b1) (bvValue $ belt !! b2)
interpInstr belt dmem smem pc (Imul b1 b2) =
  ((dmem, smem),
   (Continue, []):(Continue, []):(Continue, []):(Continue, [hi, lo]):[])
  where
    x :: Word32 = fromIntegral $ bvValue $ belt !! b1
    y :: Word32 = fromIntegral $ bvValue $ belt !! b2
    z = x * y

    lo = BeltValue (fromIntegral z) False False
    hi = BeltValue (fromIntegral $ shiftR z 16) False False
interpInstr belt dmem smem pc (Iand b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue (x .&. y) False False
interpInstr belt dmem smem pc (Ior b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue (x .|. y) False False
interpInstr belt dmem smem pc (Ixor b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue (xor x y) False False
interpInstr belt dmem smem pc (Isll b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue (shiftL x (fromIntegral y)) False False
interpInstr belt dmem smem pc (Isrl b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue (shiftR x (fromIntegral y)) False False
interpInstr belt dmem smem pc (Isra b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    bv = BeltValue {
      bvValue = fromIntegral $ shiftR (fromIntegral x :: Int16) (fromIntegral y),
      bvCarry = False,
      bvOFlow = False
    }
interpInstr belt dmem smem pc (Iaddc b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where bv = aluAdd (bvValue $ belt !! b1) (bool 0 1 $ bvCarry $ belt !! b2)
interpInstr belt dmem smem pc (Isub b1 b2) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = bvValue $ belt !! b2
    y' = complement y
    bv1 = aluAdd y' 1
    bv2 = aluAdd x (bvValue bv1)
    bv = BeltValue {
        bvValue = bvValue bv2,
        bvCarry = bvCarry bv1 || bvCarry bv2,
        bvOFlow = bvOFlow bv1 || bvOFlow bv2
      }
interpInstr belt dmem smem pc (Iinv b1) = ((dmem, smem), (Continue, [bv]):[])
  where bv = BeltValue (complement $ bvValue $ belt !! b1) False False
interpInstr belt dmem smem pc (Ineg b1) = ((dmem, smem), (Continue, [bv]):[])
  where bv = aluAdd 1 (complement $ bvValue $ belt !! b1)
interpInstr belt dmem smem pc (Ilcall1 b1) = ((dmem, frame:smem), (Jump addr, []):[])
  where
    addr = bvValue $ belt !! b1
    frame = Frame {
        frMem = IM.empty,
        frRA = pc + 1,
        frLen = 0
      }
interpInstr belt dmem smem pc (Iljmp b1) = ((dmem, smem), (Jump addr, []):[])
  where addr = bvValue $ belt !! b1
interpInstr belt dmem smem pc (Icarry b1) = ((dmem, smem), (Continue, [bv]):[])
  where bv = BeltValue (bool 0 1 $ bvCarry $ belt !! b1) False False
interpInstr belt dmem smem pc Ihalt = ((dmem, smem), (Halt, []):[])
interpInstr belt dmem smem pc Inop = ((dmem, smem), [])
interpInstr belt dmem smem pc Ipc = ((dmem, smem), (Continue, [bv]):[])
  where bv = BeltValue pc False False
interpInstr belt dmem smem@(fr:_) pc Iret1 =
  ((dmem, smem), (Jump $ frRA fr, []):(Continue, []):[])
interpInstr belt dmem (fr:smem) pc Iret2 =
  ((dmem, smem), (Continue, []):(Continue, []):[])
interpInstr belt dmem smem pc Icall2 =
  ((dmem, smem), (Continue, []):(Continue, []):[])
interpInstr belt dmem smem pc (Iaddi b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where bv = aluAdd (bvValue $ belt !! b1) (fromIntegral imm)
interpInstr belt dmem smem pc (Iandi b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = fromIntegral imm
    bv = BeltValue (x .&. y) False False
interpInstr belt dmem smem pc (Iori b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = fromIntegral imm
    bv = BeltValue (x .|. y) False False
interpInstr belt dmem smem pc (Ixori b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = fromIntegral imm
    bv = BeltValue (xor x y) False False
interpInstr belt dmem smem pc (Islli b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = fromIntegral imm
    bv = BeltValue (shiftL x y) False False
interpInstr belt dmem smem pc (Isrli b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = bvValue $ belt !! b1
    y = fromIntegral imm
    bv = BeltValue (shiftR x y) False False
interpInstr belt dmem smem pc (Israi b1 imm) = ((dmem, smem), (Continue, [bv]):[])
  where
    x = fromIntegral $ bvValue $ belt !! b1 :: Int16
    y = fromIntegral imm
    bv = BeltValue (fromIntegral $ shiftR x y) False False
interpInstr belt dmem smem pc (Ibzero b1 imm) =
  ((dmem, smem), if cond then (Jump addr, []):[] else (Continue, []):[])
  where
    addr = pc + fromIntegral imm
    cond = bvValue (belt !! b1) == 0
interpInstr belt dmem smem pc (Ibneg b1 imm) =
  ((dmem, smem), if cond then (Jump addr, []):[] else (Continue, []):[])
  where
    addr = pc + fromIntegral imm
    cond = testBit (bvValue (belt !! b1)) 15
interpInstr belt dmem smem pc (Iboflo b1 imm) =
  ((dmem, smem), if cond then (Jump addr, []):[] else (Continue, []):[])
  where
    addr = pc + fromIntegral imm
    cond = bvOFlow $ belt !! b1
interpInstr belt dmem smem pc (Ibcar b1 imm) =
  ((dmem, smem), if cond then (Jump addr, []):[] else (Continue, []):[])
  where
    addr = pc + fromIntegral imm
    cond = bvCarry $ belt !! b1
interpInstr belt dmem (fr:smem) pc (Ists b1 imm) | fromIntegral imm >= frLen fr =
  error "BAD: sts bounds check"
interpInstr belt dmem (fr:smem) pc (Ists b1 imm) =
  ((dmem, fr { frMem = write addr val $ frMem fr }:smem),
   (Continue, []):(Continue, []):[])
  where
    addr = fromIntegral imm
    val = bvValue $ belt !! b1
interpInstr belt dmem smem pc (Ild b1 imm) =
  ((dmem, smem), (Continue, []):(Continue, [bv]):[])
  where
    addr = bvValue (belt !! b1) + fromIntegral imm
    bv = BeltValue (dmem IM.! fromIntegral addr) False False
interpInstr belt dmem smem pc (Iconst imm) = ((dmem, smem), (Continue, [bv]):[])
  where bv = BeltValue (fromIntegral imm) False False
interpInstr belt dmem smem pc (Iupper imm) = ((dmem, smem), (Continue, [bv]):[])
  where bv = BeltValue (shiftL (fromIntegral imm) 16) False False
interpInstr belt dmem smem pc (Icall1 imm) = ((dmem, frame:smem), (Jump addr, []):[])
  where
    addr = pc + fromIntegral imm
    frame = Frame {
        frMem = IM.empty,
        frRA = pc + 1,
        frLen = 0
      }
interpInstr belt dmem smem pc (Ijmp imm) = ((dmem, smem), (Jump addr, []):[])
  where addr = pc + fromIntegral imm
interpInstr belt dmem (fr:smem) pc (Ilds imm) | fromIntegral imm >= frLen fr =
  error "BAD: lds bounds check"
interpInstr belt dmem smem@(fr:_) pc (Ilds imm) =
  ((dmem, smem), (Continue, []):(Continue, [bv]):[])
  where
    bv = BeltValue (frMem fr IM.! fromIntegral imm) False False
interpInstr belt dmem (fr:smem) pc (Ialloc imm) =
  ((dmem, fr { frLen = frLen fr + fromIntegral imm }:smem),
   (Continue, []):(Continue, []):[])

data State = State {
  stShutdown :: Maybe Int,
  stPC :: Addr,
  stBelt :: Belt,
  stDMem :: Memory,
  stSMem :: Stack,
  stPending :: [(Status, [BeltValue])]
} deriving (Show)
issue :: IM.IntMap Instr -> State -> State
issue imem st | Just c <- stShutdown st = st { stShutdown = Just (c - 1) }
issue imem st@(State {..}) = st {
      stDMem = dmem',
      stSMem = smem',
      stPending = zipWith (<>) pending' stPending
    }
  where
    instr = imem IM.! fromIntegral stPC
    ((dmem', smem'), pending') = interpInstr stBelt stDMem stSMem stPC instr
commit :: State -> State
commit st@(State { .. }) =
  case status of
    Continue -> st' { stPC = stPC + 1 }
    Jump addr -> st' { stPC = addr }
    Halt -> st' { stShutdown = Just 4 }
  where
    st' = st { stBelt = drops <> stBelt, stPending = pending }
    ((status, drops), pending) = case stPending of
      h:pending -> (h, pending)
      [] -> ((Continue, []), [])

run :: IM.IntMap Instr -> State -> [State]
run imem st | Just 0 <- stShutdown st = [st]
run imem st = st:run imem (commit (issue imem st))

initial :: Addr -> State
initial pc = State {
    stShutdown = Nothing,
    stPC = pc,
    stBelt = repeat (BeltValue 0 False False),
    stDMem = IM.empty,
    stSMem = [Frame {
      frMem = IM.empty,
      frRA = 0,
      frLen = 0
    }],
    stPending = repeat (Continue, [])
  }
