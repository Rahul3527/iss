#include <stdio.h>
#include <stdint.h>
// clang -S -c Behavior.c -o Behavior.bc

int32_t srf[32];

int32_t *getSrfPtr()
{
  return srf;
}

void setSrf(int32_t reg, int32_t value)
{
  srf[reg] = value;
}

int32_t getSrf(int32_t reg)
{
  return srf[reg];
}

void add32(int32_t d0, int32_t s0, int32_t s1)
{
  //srf[s0] = 10;
  //srf[s1] = 20;
  srf[d0] = srf[s0] + srf[s1];
  //printf("srf[%d] = %d\n", d0, srf[d0]);
  //printf("srf[%d] = %d\n", s0, srf[s0]);
  //printf("srf[%d] = %d\n", s1, srf[s1]);
}

void sub32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] - srf[s1];
}

void mul32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] * srf[s1];
}

void mov32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0];
}

void dec32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[d0] - 1;
}

void eq32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] == srf[s1]);
}

void lt32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] < srf[s1]);
}

void gt32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] > srf[s1]);
}

void lte32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] <= srf[s1]);
}

void gte32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] >= srf[s1]);
}

void and32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] & srf[s1]);
}

void or32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] | srf[s1]);
}

void xor32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = (srf[s0] ^ srf[s1]);
}

void not32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = !srf[s0];
}

void lsh32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] << srf[s1];
}

void rsh32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] >> srf[s1];
}

void ld32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0];
}

void st32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[s0] = srf[s1];
}

void xchg32(int32_t d0, int32_t s0, int32_t s1)
{
  int32_t tmp = srf[0];
  srf[s0] = srf[s1];
  srf[s1] = tmp;
}

void mul32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] * srf[s1];
}

void div32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] / srf[s1];
}

void rem32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] % srf[s1];
}

void rshift32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] >> srf[s1];
}

