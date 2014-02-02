#ifndef FF_BENCH_COMMON_RANDOM_H_
#define FF_BENCH_COMMON_RANDOM_H_

#ifndef N
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
#endif

#include <memory>

struct Random {
    Random();


    void 		random_seed (unsigned long seed);

    unsigned long    random_generate ();

    unsigned long mt[N];
    unsigned long mti;

    
protected:
  
    static void init_genrand(unsigned long mt[], unsigned long * mtiPtr, unsigned long s);
    static unsigned long genrand_int32(unsigned long mt[], unsigned long * mtiPtr);
};

typedef std::shared_ptr<Random> Random_ptr;


#endif