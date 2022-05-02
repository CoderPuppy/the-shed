{-# LANGUAGE RecursiveDo, BlockArguments #-}

module SHEDHS.TestProgs where

import Prelude hiding (const)

import SHEDHS.DSL

fibRec :: Prog p (EBB p)
fibRec = mdo
  base0 <- ebb "fibRec(0)" \(_) -> do
    r <- const 0
    ret [Just r]
  base1 <- ebb "fibRec(1)" \(_) -> do
    r <- const 1
    ret [Just r]
  fib <- ebb "fibRec" \(n:_) -> do
    bzero n base0 []
    n_1 <- addi n (-1)
    bzero n_1 base1 []
    alloc 1
    sts n_1 0
    (f_1:_) <- call fib [Just n_1]
    n_1 <- lds 0
    nop
    n_2 <- addi n_1 (-1)
    sts f_1 0
    (f_2:_) <- call fib [Just n_2]
    f_1 <- lds 0
    nop
    f <- add f_1 f_2
    ret [Just f]
  pure fib

fibLoop :: Prog p (EBB p)
fibLoop = mdo
  done <- ebb "fibLoop.done" \(_:a:_) -> do
    a <- addi a 0
    ret [Just a]
  loop <- ebb "fibLoop.loop" \(b:a:n:_) -> do
    bzero n done [Nothing, Just a]
    n' <- addi n (-1)
    a' <- addi b 0
    b' <- add a b
    jmp loop [Just b', Just a', Just n']
  ebb "fibLoop" \(n:_) -> do
    a <- const 0
    b <- const 1
    jmp loop [Just b, Just a, Just n]

factorial :: Prog p (EBB p)
factorial = mdo
  done <- ebb "factorial.done" \(f:_) -> do
    ret [Just f]
  loop <- ebb "factorial.loop" \(f:n:_) -> do
    bzero n done [Just f]
    (hi, lo) <- mul f n
    nop
    nop
    n' <- addi n (-1)
    f' <- addi lo 0
    jmp loop [Just f', Just n']
  ebb "factorial" \(n:_) -> do
    f <- const 1
    jmp loop [Just f, Just n]

test_fib_fact :: Prog p (EBB p)
test_fib_fact = mdo
  fibLoop' <- fibLoop
  fibRec' <- fibRec
  fact <- factorial

  done <- ebb "done" \(_) -> do
    lo <- const 0
    hi <- const 43
    ddump lo hi
    halt
  loop <- ebb "loop" \(a:_) -> do
    e <- addi a (-11 * 4)
    bzero e done []

    n <- srli a 2
    (f:_) <- call fibRec' [Just n]
    a <- lds 0
    nop
    st f (a, 0)

    n <- srli a 2
    (f:_) <- call fibLoop' [Just n]
    a <- lds 0
    nop
    st f (a, 1)

    n <- srli a 2
    (f:_) <- call fact [Just n]
    a <- lds 0
    nop
    st f (a, 2)

    a' <- addi a 4
    sts a' 0
    jmp loop [Just a']
  ebb "start" \(_) -> do
    n <- const 0
    alloc 1
    sts n 0
    jmp loop [Just n]
