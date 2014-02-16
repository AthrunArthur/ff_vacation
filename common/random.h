#ifndef FF_BENCH_COMMON_RANDOM_H_
#define FF_BENCH_COMMON_RANDOM_H_



#include <memory>

struct Random {
    Random();


    void 		random_seed (unsigned long seed);

    unsigned long    random_generate ();

    unsigned long mt[624];
    unsigned long mti;

    
protected:
  
    static void init_genrand(unsigned long mt[], unsigned long * mtiPtr, unsigned long s);
    static unsigned long genrand_int32(unsigned long mt[], unsigned long * mtiPtr);
};

typedef std::shared_ptr<Random> Random_ptr;


#endif