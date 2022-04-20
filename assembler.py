import sys


def beltPosToBin(beltPos):
    return "{0:03b}".format(int(beltPos))

def twos_comp(val, bits):
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)        # compute negative value
    return val                         # return positive value as is

def immToBin(imm):
    if(imm.startswith("0x")):
        imm = imm.replace("0x", "", 1)
        return "{0:08b}".format(int(imm, 16))
    elif(imm.startswith("0b")):
        imm = imm.replace("0b", "", 1)
        return "{0:08b}".format(int(imm, 2))
    elif(imm.startswith("-")):
        # imm = imm.replace("-", "", 1)
        val = int(imm,10)
        s = bin(val + (1<<8)).replace("0b", "", 1)
        return "{0}".format(s)
    else:
        return "{0:08b}".format(int(imm, 10))
    

def binaryToHex(bin):
    return hex(int(bin, 2)).replace("0x", "", 1)

instrsFormat = {     # "00 000 000 0000 0000"
    "store"          : "00   B   B         I",
    "add"            : "01   B   B 0000 0000",
    "mul"            : "01   B   B 0000 0001",
    "and"            : "01   B   B 0000 0010",
    "or"             : "01   B   B 0000 0011",
    "xor"            : "01   B   B 0000 0100",
    "lshift"         : "01   B   B 0000 0101",
    "rshift"         : "01   B   B 0000 0110",
    "arshift"        : "01   B   B 0000 0111",
    "addc"           : "01   B   B 0000 1000",
    "sub"            : "01   B   B 0000 1001",
    "subc"           : "01   B   B 0000 1010",
    "invert"         : "01   B 000 1111 1111",
    "negate"         : "01   B 001 1111 1111",
    "longcall"       : "01   B 010 1111 1111",
    "longjump"       : "01   B 011 1111 1111",
    "get_carry"      : "01   B 100 1111 1111",
    "halt"           : "01 000 111 1111 1111",
    "nop"            : "01 001 111 1111 1111",
    "return"         : "01 010 111 1111 1111",
    "get_pc"         : "01 011 111 1111 1111",
    "addi"           : "10   B 000         I",
    "muli"           : "10   B 001         I",
    "andi"           : "10   B 010         I",
    "ori"            : "10   B 011         I",
    "xori"           : "10   B 100         I",
    "lshifti"        : "10   B 101         I",
    "rshift1"        : "10   B 110         I",
    "arshifti"       : "10   B 111         I",
    "branch_zero"    : "11   B 000         I",
    "branch_neg"     : "11   B 001         I",
    "branch_oflow"   : "11   B 010         I",
    "branch_carry"   : "11   B 011         I",
    "write_stack"    : "11   B 100         I",
    "load"           : "11   B 101         I",
    "constant"       : "11 000 111         I",
    "upper"          : "11 001 111         I",
    "call"           : "11 010 111         I",
    "jump"           : "11 011 111         I",
    "read_stack"     : "11 100 111         I",
    "alloca"         : "11 101 111         I"
}


def assembleInstruction(instruction):
    parts = instruction.replace("\n", "").split(" ")
    format = instrsFormat[parts[0]].replace(" ", "")
    argsIndex = 1
    if(format.count("B") > 0):
        format = format.replace("B", beltPosToBin(parts[argsIndex]), 1)
        argsIndex += 1
    if(format.count("B") > 0):
        format = format.replace("B", beltPosToBin(parts[argsIndex]), 1)
        argsIndex += 1
    if(format.count("I") == 1):
        format = format.replace("I", immToBin(parts[argsIndex]), 1)
        argsIndex += 1
    return binaryToHex(format)

def formatOutput(memLoc, instr):
    return "{0:02X} 1 {1}\n".format(memLoc, instr)

def assembleFile(inputFileName):
    outputFileName = inputFileName[0 : inputFileName.rindex('.')] + ".obj"
    inF = open(inputFileName, "r")
    outF = open(outputFileName, "w")

    memPos = 0

    for line in inF:
        if line.replace(" ", "") != "":
            outF.write(formatOutput(memPos, assembleInstruction(line)))
            memPos += 1
    outF.write("00")
    inF.close()
    outF.close()


assembleFile(sys.argv[1])