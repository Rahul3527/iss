#include <stdio.h>
#include <stdint.h>

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
  srf[s0] = 10;
  srf[s1] = 20;
  srf[d0] = srf[s0] + srf[s1];
  printf("srf[%d] = %d\n", d0, srf[d0]);
}

void sub32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] - srf[s1];
}

void mul32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] * srf[s1];
}

void rshift32(int32_t d0, int32_t s0, int32_t s1)
{
  srf[d0] = srf[s0] >> srf[s1];
}

