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

extern struct nunrand_s D_0062EA60;
extern long D_0062EA68;
#define global_rand D_0062EA60
#define fseed D_0062EA68

long int __divdi3 (long int u, long int v);

void NuRandSetSeed(struct nunrand_s *nrand,int seed) {
  struct nunrand_s *temp;
  
  temp = &global_rand;
  if (nrand != 0) {
    temp = nrand;
  }
  temp->idum = seed;
}

long NuRand(struct nunrand_s *nrand) {
    long seed;
    long quotient;

    if (nrand == 0) {
        nrand = &global_rand;
    }
    seed = nrand->idum ^ 0x075BD924;
    nrand->idum = seed;
    quotient = seed / 0x31E5;
    nrand->idum = (seed - quotient * 0x31E5) * 0x41A7 - quotient * 0xB14;
    if (nrand->idum < 0) {
        nrand->idum += 0x7FFFFFFF;
    }
    nrand->idum ^= 0x075BD924;
    return nrand->idum;
}


float NuFloatRand(struct nunrand_s *nrand) {
    long quotient;

    if (nrand == 0) {
        nrand = &global_rand;
    }
    nrand->idum ^= 0x075BD924;
    quotient = __divdi3(nrand->idum, 0x31E5);
    nrand->idum = (nrand->idum - quotient * 0x31E5) * 0x41A7 - quotient * 0xB14;
    if (nrand->idum < 0) {
        nrand->idum += 0x7FFFFFFF;
    }
    nrand->idum ^= 0x075BD924;
    return nrand->idum * 4.656613e-10f;
}


float NuRandFloat(void) {
    union {
        float f;
        int i;
    } bits;

    fseed = fseed * 0x19660D + 0x3C6EF35F;
    bits.i = (fseed & 0x7FFFFF) | 0x3F800000;
    return bits.f - 1.0f;
}


int NuRandInt(void) {
    fseed = fseed * 0x19660D + 0x3C6EF35F;
    return fseed;
}


void NuRandSeed(unsigned int seed) {
  fseed = seed;
}
