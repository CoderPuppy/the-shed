{-# LANGUAGE BinaryLiterals, NumericUnderscores #-}

module SHEDHS.Assembler where

import Data.Word
import Data.Bits

import SHEDHS.Instruction

assemble' :: Instr -> (Int, Int, Int, Word8)
assemble' (Ist  a b i) = (0b00,     a,     b, fromIntegral i)
assemble' (Iadd   a b) = (0b01,     a,     b, 0b0000_0000)
assemble' (Imul   a b) = (0b01,     a,     b, 0b0000_0001)
assemble' (Iand   a b) = (0b01,     a,     b, 0b0000_0010)
assemble' (Ior    a b) = (0b01,     a,     b, 0b0000_0011)
assemble' (Ixor   a b) = (0b01,     a,     b, 0b0000_0100)
assemble' (Isll   a b) = (0b01,     a,     b, 0b0000_0101)
assemble' (Isrl   a b) = (0b01,     a,     b, 0b0000_0110)
assemble' (Isra   a b) = (0b01,     a,     b, 0b0000_0111)
assemble' (Iaddc  a b) = (0b01,     a,     b, 0b0000_1000)
assemble' (Isub   a b) = (0b01,     a,     b, 0b0000_1001)
assemble' (Iddump a b) = (0b01,     a,     b, 0b0000_1010)
assemble' (Isdump a b) = (0b01,     a,     b, 0b0000_1011)
assemble' (Iinv     a) = (0b01,     a, 0b000, 0b1111_1111)
assemble' (Ineg     a) = (0b01,     a, 0b001, 0b1111_1111)
assemble' (Ilcall1  a) = (0b01,     a, 0b010, 0b1111_1111)
assemble' (Iljmp    a) = (0b01,     a, 0b011, 0b1111_1111)
assemble' (Icarry   a) = (0b01,     a, 0b100, 0b1111_1111)
assemble'  Ihalt       = (0b01, 0b000, 0b111, 0b1111_1111)
assemble'  Inop        = (0b01, 0b001, 0b111, 0b1111_1111)
assemble'  Ipc         = (0b01, 0b010, 0b111, 0b1111_1111)
assemble'  Iret1       = (0b01, 0b011, 0b111, 0b1111_1111)
assemble'  Iret2       = (0b01, 0b100, 0b111, 0b1111_1111)
assemble'  Icall2      = (0b01, 0b101, 0b111, 0b1111_1111)
assemble' (Iaddi  a i) = (0b10,     a, 0b000, fromIntegral i)
assemble' (Imuli  a i) = (0b10,     a, 0b001, fromIntegral i)
assemble' (Iandi  a i) = (0b10,     a, 0b010, fromIntegral i)
assemble' (Iori   a i) = (0b10,     a, 0b011, i)
assemble' (Ixori  a i) = (0b10,     a, 0b100, i)
assemble' (Islli  a i) = (0b10,     a, 0b101, i)
assemble' (Isrli  a i) = (0b10,     a, 0b110, i)
assemble' (Israi  a i) = (0b10,     a, 0b111, i)
assemble' (Ibzero a i) = (0b11,     a, 0b000, fromIntegral i)
assemble' (Ibneg  a i) = (0b11,     a, 0b001, fromIntegral i)
assemble' (Iboflo a i) = (0b11,     a, 0b010, fromIntegral i)
assemble' (Ibcar  a i) = (0b11,     a, 0b011, fromIntegral i)
assemble' (Ists   a i) = (0b11,     a, 0b100, i)
assemble' (Ild    a i) = (0b11,     a, 0b101, fromIntegral i)
assemble' (Iconst   i) = (0b11, 0b000, 0b111, fromIntegral i)
assemble' (Iupper   i) = (0b11, 0b001, 0b111, i)
assemble' (Icall1   i) = (0b11, 0b010, 0b111, fromIntegral i)
assemble' (Ijmp     i) = (0b11, 0b011, 0b111, fromIntegral i)
assemble' (Ilds     i) = (0b11, 0b100, 0b111, i)
assemble' (Ialloc   i) = (0b11, 0b101, 0b111, i)

assemble :: Instr -> Word16
assemble instr
  =   shiftL (fromIntegral field1) 14
  .|. shiftL (fromIntegral field2) 11
  .|. shiftL (fromIntegral field3)  8
  .|. shiftL (fromIntegral field4)  0
  where (field1, field2, field3, field4) = assemble' instr
