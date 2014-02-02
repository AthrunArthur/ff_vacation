#include "random.h"

#define RANDOM_DEFAULT_SEED (0)


/* initializes mt[N] with a seed */
Random::Random()
: mti(N)
{
init_genrand(mt, &mti, RANDOM_DEFAULT_SEED);
}

void Random::init_genrand(long unsigned int mt[], long unsigned int* mtiPtr, long unsigned int s)
{

    unsigned long mti;

    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
          (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }

    (*mtiPtr) = mti;
}

long unsigned int Random::genrand_int32(long unsigned int mt[], long unsigned int* mtiPtr)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    unsigned long mti = (*mtiPtr);

    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        long kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(mt, mtiPtr, 5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    (*mtiPtr) = mti;

    return y;
}



long unsigned int Random::random_generate()
{
  return genrand_int32(mt, &mti);
}

void Random::random_seed(long unsigned int seed)
{
init_genrand(mt, &mti, seed);
}
