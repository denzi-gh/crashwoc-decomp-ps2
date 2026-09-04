/*
 * Unit: numath/nurand
 *
 * Functions:
 *   0x0010e458 NuRandSetSeed
 *   0x0010e468 NuRand
 *   0x0010e548 NuFloatRand
 *   0x0010e630 NuRandFloat
 *   0x0010e6a8 NuRandInt
 *   0x0010e6f8 NuRandSeed
 */

struct nunrand_s {
    long idum;
};

struct nunrand_s global_rand;
static long fseed;

long int __divdi3 (long int u, long int v);
float __floatdisf (long i);

void NuRandSetSeed(struct nunrand_s *nrand,int seed) {
  struct nunrand_s *temp;
  
  temp = &global_rand;
  if (nrand != 0) {
    temp = nrand;
  }
  temp->idum = seed;
}

float NuRandFloat(void) {
  fseed = (fseed * 0x19660d) + 0x3C6EF35F;
  return (float)((int)(fseed & 0x7fffff) | 0x3f800000) - 1.0f;
}

unsigned int NuRandInt(void) {
  fseed = fseed * 0x19660d + 0x3c6ef35f;
  return fseed;
}

void NuRandSeed(unsigned int seed) {
  fseed = seed;
}