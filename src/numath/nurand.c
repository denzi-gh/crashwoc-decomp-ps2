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

long int __divdi3 (long int u, long int v);

long NuRand(struct nunrand_s* nrand) {
    struct nunrand_s* temp;
    long temp_s0;
    long temp_v0;

    temp = (nrand != 0) ? nrand : &global_rand;
    temp_s0 = temp->idum ^ 0x075BD924;
    temp->idum = temp_s0;
    temp_v0 = __divdi3(temp_s0, 0x31E5);
    temp->idum = ((temp_s0 - (temp_v0 * 0x31E5)) * 0x41A7) - (temp_v0 * 0xB14);
    if (temp->idum < 0) {
        temp->idum = (long) (temp->idum + 0x7FFFFFFF);
    }
    temp->idum = (long) (temp->idum ^ 0x075BD924);
    return temp->idum;
}

f32 NuFloatRand(struct nunrand_s * nrand) {
    long temp_v0;
    struct nunrand_s* temp;

    temp = (nrand != 0) ? nrand : &global_rand;
    temp->idum = temp->idum ^ 0x075BD924;
    temp_v0 = __divdi3(temp->idum, 0x31E5);
    temp->idum = ((temp->idum - (temp_v0 * 0x31E5)) * 0x41A7) - (temp_v0 * 0xB14);
    if (temp->idum < 0) {
        temp->idum = temp->idum + 0x7FFFFFFF;
    }
    temp->idum = temp->idum ^ 0x075BD924;
    return __floatdisf((int) temp->idum) * 4.656613e-10f;
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