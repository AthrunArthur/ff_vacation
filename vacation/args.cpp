#include "args.h"
#include <boost/format.hpp>
#include <iostream>

std::shared_ptr<params> params::s_pInstance(nullptr);

params& params::instance()
{
  return * s_pInstance.get();
}

params::params()
: m_oOpDes("Vacation Benchmark for Function Flow (athrunarthur@gmail.com) ")
{
  m_oOpDes.add_options()
  ("clients", boost::program_options::value<int>(), boost::str(boost::format("Number of clients, default: %1%")%PARAM_DEFAULT_CLIENTS).c_str())
  ("qtr", boost::program_options::value<int>(), boost::str(boost::format("Number of user queries/transaction, default: %1%1")%PARAM_DEFAULT_NUMBER).c_str())
  ("query", boost::program_options::value<int>(), boost::str(boost::format("Percentage of relations queried, default is %1%")%PARAM_DEFAULT_QUERIES).c_str())
  ("relation", boost::program_options::value<int>(), 
    boost::str(boost::format("Number of possible relations, default is %1%")%PARAM_DEFAULT_RELATIONS).c_str())
  ("transaction", boost::program_options::value<int>(),
    boost::str(boost::format("Number of transactions, default is %1%")%PARAM_DEFAULT_TRANSACTIONS).c_str())
  ("user", boost::program_options::value<int>(),
    boost::str(boost::format("Percentage of user transactions, default is %1%")%PARAM_DEFAULT_USER).c_str())
  ("thread", boost::program_options::value<int>(),
    boost::str(boost::format("Number of threads, default is %1%")%PARAM_DEFAULT_THREADS).c_str())
  ("ff-lock", "Using Function Flow lock")
  ("help", "Print this help information.");
  
}

int params::parse_args(long int argc, char*const argv[])
{
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_oOpDes), m_oVars);
  boost::program_options::notify(m_oVars);
  
  if(m_oVars.count("help"))
  {
    std::cout<<m_oOpDes<<std::endl;
    return -1;
  }
#define ADD_OPTION(name, type, dv)	if(m_oVars.count(name)) \
    {  \
      m_oParams.insert(std::make_pair(type, m_oVars[name].as<int>())); \
    } \
    else m_oParams.insert(std::make_pair(type, dv));
    
    ADD_OPTION("clients", P_CLIENTS, PARAM_DEFAULT_CLIENTS)
    ADD_OPTION("qtr", P_NUMBER, PARAM_DEFAULT_NUMBER)
    ADD_OPTION("query", P_QUERIES, PARAM_DEFAULT_QUERIES)
    ADD_OPTION("relation", P_RELATIONS, PARAM_DEFAULT_RELATIONS)
    ADD_OPTION("transaction", P_TRANSACTIONS, PARAM_DEFAULT_TRANSACTIONS)
    ADD_OPTION("user", P_USER, PARAM_DEFAULT_USER)
    ADD_OPTION("thread", P_THREADS, PARAM_DEFAULT_THREADS)
    if(m_oVars.count("ff-lock"))
    {
      m_oParams.insert(std::make_pair(P_FF_LOCK, 1));
    }
    else m_oParams.insert(std::make_pair(P_FF_LOCK, 0));
    
#undef ADD_OPTION
      
  return 0;
}

int params::operator[](param_types pt)
{
  return m_oParams[pt];
}

int params::parseArgs(long int argc, char*const argv[])
{
  s_pInstance = std::shared_ptr<params>(new params());
  return s_pInstance->parse_args(argc, argv);
}

