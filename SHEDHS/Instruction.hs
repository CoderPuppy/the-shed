module SHEDHS.Instruction where

import Data.Int
import Data.Word

type SEImm = Int8
type ZEImm = Word8
type BeltOp = Int

data Instr
  = Ist BeltOp BeltOp SEImm
  | Iadd BeltOp BeltOp
  | Imul BeltOp BeltOp
  | Iand BeltOp BeltOp
  | Ior BeltOp BeltOp
  | Ixor BeltOp BeltOp
  | Isll BeltOp BeltOp
  | Isrl BeltOp BeltOp
  | Isra BeltOp BeltOp
  | Iaddc BeltOp BeltOp
  | Isub BeltOp BeltOp
  | Iinv BeltOp
  | Ineg BeltOp
  | Ilcall1 BeltOp
  | Iljmp BeltOp
  | Icarry BeltOp
  | Ihalt
  | Inop
  | Ipc
  | Iret1
  | Iret2
  | Icall2
  | Iaddi BeltOp SEImm
  | Imuli BeltOp SEImm
  | Iandi BeltOp SEImm
  | Iori BeltOp ZEImm
  | Ixori BeltOp ZEImm
  | Islli BeltOp ZEImm
  | Isrli BeltOp ZEImm
  | Israi BeltOp ZEImm
  | Ibzero BeltOp SEImm
  | Ibneg BeltOp SEImm
  | Iboflo BeltOp SEImm
  | Ibcar BeltOp SEImm
  | Ists BeltOp ZEImm
  | Ild BeltOp SEImm
  | Iconst SEImm
  | Iupper ZEImm
  | Icall1 SEImm
  | Ijmp SEImm
  | Ilds ZEImm
  | Ialloc ZEImm
  deriving (Show, Eq)

latency :: Instr -> Int
latency (Ist  _ _ _) = 2
latency (Iadd   _ _) = 1
latency (Imul   _ _) = 4
latency (Iand   _ _) = 1
latency (Ior    _ _) = 1
latency (Ixor   _ _) = 1
latency (Isll   _ _) = 1
latency (Isrl   _ _) = 1
latency (Isra   _ _) = 1
latency (Iaddc  _ _) = 1
latency (Isub   _ _) = 1
latency (Iinv     _) = 1
latency (Ineg     _) = 1
latency (Ilcall1  _) = 2
latency (Iljmp    _) = 2
latency (Icarry   _) = 1
latency  Ihalt       = 0
latency  Inop        = 0
latency  Ipc         = 1
latency  Iret1       = 2
latency  Iret2       = 2
latency  Icall2      = 2
latency (Iaddi  _ _) = 1
latency (Imuli  _ _) = 4
latency (Iandi  _ _) = 1
latency (Iori   _ _) = 1
latency (Ixori  _ _) = 1
latency (Islli  _ _) = 1
latency (Isrli  _ _) = 1
latency (Israi  _ _) = 1
latency (Ibzero _ _) = 2
latency (Ibneg  _ _) = 2
latency (Iboflo _ _) = 2
latency (Ibcar  _ _) = 2
latency (Ists   _ _) = 2
latency (Ild    _ _) = 2
latency (Iconst   _) = 1
latency (Iupper   _) = 1
latency (Icall1   _) = 2
latency (Ijmp     _) = 2
latency (Ilds     _) = 2
latency (Ialloc   _) = 2

numDrops :: Instr -> Int
numDrops (Ist  _ _ _) = 0
numDrops (Iadd   _ _) = 1
numDrops (Imul   _ _) = 2
numDrops (Iand   _ _) = 1
numDrops (Ior    _ _) = 1
numDrops (Ixor   _ _) = 1
numDrops (Isll   _ _) = 1
numDrops (Isrl   _ _) = 1
numDrops (Isra   _ _) = 1
numDrops (Iaddc  _ _) = 1
numDrops (Isub   _ _) = 1
numDrops (Iinv     _) = 1
numDrops (Ineg     _) = 1
numDrops (Ilcall1  _) = 0
numDrops (Iljmp    _) = 0
numDrops (Icarry   _) = 1
numDrops  Ihalt       = 0
numDrops  Inop        = 0
numDrops  Ipc         = 1
numDrops  Iret1       = 0
numDrops  Iret2       = 0
numDrops  Icall2      = 0
numDrops (Iaddi  _ _) = 1
numDrops (Imuli  _ _) = 2
numDrops (Iandi  _ _) = 1
numDrops (Iori   _ _) = 1
numDrops (Ixori  _ _) = 1
numDrops (Islli  _ _) = 1
numDrops (Isrli  _ _) = 1
numDrops (Israi  _ _) = 1
numDrops (Ibzero _ _) = 0
numDrops (Ibneg  _ _) = 0
numDrops (Iboflo _ _) = 0
numDrops (Ibcar  _ _) = 0
numDrops (Ists   _ _) = 0
numDrops (Ild    _ _) = 1
numDrops (Iconst   _) = 1
numDrops (Iupper   _) = 1
numDrops (Icall1   _) = 0
numDrops (Ijmp     _) = 0
numDrops (Ilds     _) = 1
numDrops (Ialloc   _) = 0
