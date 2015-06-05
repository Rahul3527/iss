
enum OPCODE {
  ADD32,
  SUB32,
  MUL32,
  MOV32,
  DEC32,
  EQ32,
  LT32,
  GT32,
  LTE32,
  GTE32,
  AND32,
  OR32,
  XOR32,
  NOT32,
  LSH32,
  RSH32,
  LD32,
  ST32,
  XCHG32, 
  DIV32,
  REM32,
  JMP
};

struct Instruction
{
  Instruction(OPCODE op, int32_t d0, int32_t s0, int32_t s1)
  {
    opcode = op;
    dst_reg = d0;
    src0_reg = s0;
    src1_reg = s1;
  }
  OPCODE opcode;
  int32_t dst_reg;
  int32_t src0_reg;
  int32_t src1_reg;
};

