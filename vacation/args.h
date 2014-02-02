#ifndef FF_BENCH_VACATION_ARGS_H_
#define FF_BENCH_VACATION_ARGS_H_



enum param_types {
    PARAM_CLIENTS      = (unsigned char)'c',
    PARAM_NUMBER       = (unsigned char)'n',
    PARAM_QUERIES      = (unsigned char)'q',
    PARAM_RELATIONS    = (unsigned char)'r',
    PARAM_TRANSACTIONS = (unsigned char)'t',
    PARAM_USER         = (unsigned char)'u',
};

#define PARAM_DEFAULT_CLIENTS      (1)
#define PARAM_DEFAULT_NUMBER       (10)
#define PARAM_DEFAULT_QUERIES      (90)
#define PARAM_DEFAULT_RELATIONS    (1 << 16)
#define PARAM_DEFAULT_TRANSACTIONS (1 << 26)
#define PARAM_DEFAULT_USER         (80)


extern double global_params[256]; /* 256 = ascii limit */

void displayUsage (const char* appName);
void setDefaultParams ();

void parseArgs (long argc, char* const argv[]);


#endif