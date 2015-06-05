#include <map>
#include <string>
#include <cstdio>

enum OPCODE {
  NOP = 0,
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
  JMP,
  INVALID
};

struct CInst
{
  CInst() {}
  CInst(OPCODE op, int32_t d0, int32_t s0, int32_t s1)
  {
    opcode = op;
    dst_reg = d0;
    src0_reg = s0;
    src1_reg = s1;
  }
  void Print()
  {
    std::cout << opcode << "(" << dst_reg << ", " << src0_reg << ", " << src1_reg << ")" << std::endl;

  }
  OPCODE opcode;
  int32_t dst_reg;
  int32_t src0_reg;
  int32_t src1_reg;
};

std::map<OPCODE, std::string> op_map;
//void InitOperations(std::map<OPCODE, std::string> &op_map)
void InitOperations()
{
    op_map[  NOP ] = "nop";
    op_map[  ADD32 ] = "add32";
    op_map[  SUB32 ] = "sub32";
    op_map[  MUL32 ] = "mul32";
    op_map[  MOV32 ] = "mov32";
    op_map[  DEC32 ] = "dec32";
    op_map[  EQ32 ] = "eq32";
    op_map[  LT32 ] = "lt32";
    op_map[  GT32 ] = "gt32";
    op_map[  LTE32 ] = "lt32";
    op_map[  GTE32 ] = "gte32"; 
    op_map[  AND32 ] = "and32";
    op_map[  OR32 ] = "or32";
    op_map[  XOR32 ] = "xor32";
    op_map[  NOT32 ] = "not32";
    op_map[  LSH32 ] = "lsh32";
    op_map[  RSH32 ] = "rsh32";
    op_map[  LD32 ] = "ld32";
    op_map[  ST32 ] = "st32";
    op_map[  XCHG32 ] = "xchg32";
    op_map[  DIV32 ] = "div32";
    op_map[  REM32 ] = "rem32";
    op_map[  JMP ] = "jmp";
}

std::string GetFuncName(OPCODE opcode)
{
  return op_map[opcode];
}

