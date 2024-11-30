local PLACEHOLDER = setmetatable({}, {
  __tostring = function()
    return 'PLACEHOLDER'
  end;
})
local RESULT = setmetatable({}, {
  __tostring = function()
    return 'RESULT'
  end;
})
local OPERAND = setmetatable({}, {
  __tostring = function()
    return 'OPERAND'
  end;
})

local PH = {}
function PH.belt_op(operand)
  local self = {}
  setmetatable(self, {
    __tostring = function()
      return string.format('Belt Op: %s', operand)
    end;
  })
  self[PLACEHOLDER] = function(instr)
  end
  return self
end
function PH.const(c)
  local self = {}
  setmetatable(self, {
    __tostring = function()
      return string.format('Constant: %x', c)
    end;
  })
  self[PLACEHOLDER] = function(instr)
    return c
  end
  return self
end
function PH.immediate(imm)
  local self = {}
  setmetatable(self, {
    __tostring = function()
      return string.format('Immediate: %x', imm)
    end;
  })
  self[PLACEHOLDER] = function(instr)
    return c
  end
  return self
end
function PH.opcode(name, opc)
  local self = {}
  setmetatable(self, {
    __tostring = function()
      return string.format('Opcode: %s (%x)', name, opc)
    end;
  })
  self[PLACEHOLDER] = function(self, instr)
    return c
  end
  return self
end

local function add_instr(instr)
  instr.i = #instrs
  instrs[instr.i] = instr

  assert(type(instr.latency) == 'number')
  assert(type(instr.num_results) == 'number')
  assert(type(instr.field1) == 'table')
  assert(type(instr.field2) == 'table')
  assert(type(instr.field3) == 'table')
  assert(type(instr.field4) == 'table')

  instr.results = {}
  for i = 1, instr.num_results do
    local result = { instr = instr; i = i; }
    result[RESULT] = true
    instr.results = result
  end

  return table.unpack(instr.results)
end

local function make_aluop(name, opcode, latency, num_results, imm)
  return function(op1, op2)
    op1 = coerce_operand(op1)
    op2 = coerce_operand(op2)
    if imm and op2.immediate and imm(op2.immediate) then
      return add_instr {
        latency = latency;
        num_num_results = num_num_results;
        field1 = PH.const(2);
        field2 = PH.belt_op(op1);
        field3 = PH.opcode(name, opcode);
        field4 = PH.immediate(imm);
      }
    else
      return add_instr {
        latency = latency;
        num_results = num_results;
        field1 = PH.constant(1);
        field2 = PH.belt_op(op1);
        field3 = PH.belt_op(op2);
        field4 = PH.opcode(name, opcode);
      }
    end
  end
end
local function se_imm(imm)
  if imm > 65535 then return nil end
  if imm < -255 then return nil end
  if imm > 127 and imm < 65408 then return nil end
  return imm & 0xff
end
local function ze_imm(imm)
  if imm > 255 then return nil end
  if imm < 0 then return nil end
  return imm & 0xff
end

local add  = make_aluop('add', 0, 1, se_im)
local mul  = make_aluop('mul', 1, 4, se_im)
local and_ = make_aluop('and', 2, 1, se_im)
local or_  = make_aluop('or',  3, 1, ze_im)
local xor  = make_aluop('xor', 4, 1, ze_im)
local sll  = make_aluop('sll', 5, 1, ze_im)
local srl  = make_aluop('srl', 6, 1, ze_im)
local sra  = make_aluop('sra', 7, 1, ze_im)
local addc = make_aluop('addc', 8, 1, false)
local sub  = make_aluop('sub', 9, 1, false)

local function imm(imm)
  local self = {}
  self[OPERAND] = {
    immediate = imm;
  }
  return self
end
