#ifndef FF_BENCH_VACATION_ARGS_H_
#define FF_BENCH_VACATION_ARGS_H_
#include <boost/program_options.hpp>
#include <memory>
#include <map>

enum param_types {
    P_CLIENTS      = (unsigned char)'c',
    P_NUMBER       = (unsigned char)'n',
    P_QUERIES      = (unsigned char)'q',
    P_RELATIONS    = (unsigned char)'r',
    P_TRANSACTIONS = (unsigned char)'t',
    P_USER         = (unsigned char)'u',
    P_THREADS     = (unsigned char) 'd',
    P_FF_LOCK   = (unsigned char) 'l',
};
#define PARAM_DEFAULT_CLIENTS      (1)
#define PARAM_DEFAULT_NUMBER       (10)
#define PARAM_DEFAULT_QUERIES      (90)
#define PARAM_DEFAULT_RELATIONS    (1 << 16)
#define PARAM_DEFAULT_TRANSACTIONS (1 << 26)
#define PARAM_DEFAULT_USER         (80)
#define PARAM_DEFAULT_THREADS		(4)


/*
 * Usage: params::instance()[P_CLIENTS] 
*/
class params{
public:
  static  params &  instance();
  
  static 	int 	parseArgs(long argc, char * const argv[]);
  
  int			operator[](param_types pt);
  
protected:
  params();
  
  int			parse_args(long argc, char *const argv[]);
  
protected:
  static std::shared_ptr<params> 	s_pInstance;
  boost::program_options::options_description m_oOpDes;
  boost::program_options::variables_map		m_oVars;
  std::map<param_types, int>	m_oParams;
  
  
};







extern double global_params[256]; /* 256 = ascii limit */

void displayUsage (const char* appName);
void setDefaultParams ();

void parseArgs (long argc, char* const argv[]);


#endif