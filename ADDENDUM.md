# Addendum to Hardware & ISA Documents for the SHED
Author: Drew Young

## Hardware

We made a minor modification to the the multiplier, adding another ALU
for the 0th bit. The ALU only ever does left shift by 0 and so is
essentially pointless, but having it makes the implementation easier.

## ISA

### Shorten Mnemonics

We found the length of mnemonics to be a problem for printing in
columns, so we shortened several of them.

- `store` -> `st`
- `lshift` -> `sll`
- `rshift` -> `srl`
- `arshift` -> `sra`
- `invert` -> `inv`
- `negate` -> `neg`
- `longcall` -> `lcall` (also split, see below)
- `longjump` -> `ljmp`
- `get_carry` -> `carry`
- `return` -> `ret` (also split, see below)
- `get_pc` -> `pc`
- `lshifti` -> `slli`
- `rshifti` -> `srli`
- `arshifti` -> `srai`
- `branch_zero` -> `bzero`
- `branch_neg` -> `bneg`
- `branch_oflo` -> `boflo`
- `branch_carry` -> `bcar`
- `write_stack` -> `sts`
- `load` -> `ld`
- `constant` -> `const`
- `jump` -> `jmp`
- `read_stack` -> `lds`
- `alloca` -> `alloc`

### Remove Subtract with Carry

We removed the subtract with carry instruction (`subc`) because its
purpose is already covered by `addc`.

### Add Memory Dump Instructions

We added two instructions: `ddump` and `sdump`. These dump data memory
and stack memory respectively. They each take two belt operands which
specify the range to dump.

`ddump` encoding:

     01 b1 b2 0000 1010

`sdump` encoding:

     01 b1 b2 0000 1011

### Split Call and Return

We split call, longcall and return each into two parts. Call and
longcall share the second part, so we have `call1`, `lcall1`, `call2`,
`ret1` and `ret2`. Each pair must be occur sequentially together, so
return would be `ret1` followed by `ret2`. `call1`, `lcall1` and `ret1`
keep the encodings of `call`, `lcall` and `ret` respectively.

`ret2` encoding:

    01 100 111 1111 1111

`call2` encoding

    01 101 111 1111 1111
