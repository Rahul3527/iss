#include <stdio.h>
#include <stdint.h>
// clang -emit-llvm- -c Behavior.c -o Behavior.bc

int32_t srf[32];

extern void test()
{
  srf[0] = srf[0] + srf[1];
}

extern void nop(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] + srf[s1];
}

extern void add32(int32_t d0, int32_t s0, int32_t s1)
{
  //srf[s0] = 10;
  //srf[s1] = 20;
  srf[d0] = srf[s0] + srf[s1];
  //printf("srf[%d] = %d\n", d0, srf[d0]);
  //printf("srf[%d] = %d\n", s0, srf[s0]);
  //printf("srf[%d] = %d\n", s1, srf[s1]);
}

extern void sub32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] - srf[s1];
}

extern void mul32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] * srf[s1];
}

extern void mov32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0];
}

extern void dec32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[d0] - 1;
}

extern void eq32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] == srf[s1]);
}

extern void lt32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] < srf[s1]);
}

extern void gt32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] > srf[s1]);
}

extern void lte32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] <= srf[s1]);
}

extern void gte32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] >= srf[s1]);
}

extern void and32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] & srf[s1]);
}

extern void or32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] | srf[s1]);
}

extern void xor32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] ^ srf[s1]);
}

extern void not32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = !srf[s0];
}

extern void lsh32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] << srf[s1];
}

extern void rsh32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] >> srf[s1];
}

extern void ld32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0];
}

extern void st32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[s0] = srf[s1];
}

extern void xchg32(int32_t d0, int32_t s0, int32_t s1)
{
  int32_t tmp = srf[0];
  srf[s0] = srf[s1];
  srf[s1] = tmp;
}

extern void div32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] / srf[s1];
}

extern void rem32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] % srf[s1];
}

extern void jmp(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] >> srf[s1];
}

