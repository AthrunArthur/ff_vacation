#include "args.h"
#include "client.h"
#include "customer.h"
#include "manager.h"
#include <functional>
#include <cassert>
#include <stdio.h>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <ff.h>
#include "record.h"
#ifdef CACHE_EVAL
#include <papi.h>
#include <assert.h>
#endif

#include <vector>


static Manager_ptr initializeManager ()
{
    Manager_ptr managerPtr = std::make_shared<Manager>();
    long i;
    long numRelation;
    Random_ptr randomPtr = std::make_shared<Random>();
    long* ids;
    typedef std::function<bool (long, long, long)> manager_func_t;
    typedef std::vector<manager_func_t> manager_funcs_t;
    manager_funcs_t manager_funcs;
    manager_funcs.push_back([managerPtr](long id, long num, long price){return managerPtr->add_car(id, num, price);});
    manager_funcs.push_back([managerPtr](long id, long num, long price){return managerPtr->add_flight(id, num, price);});
    manager_funcs.push_back([managerPtr](long id, long num, long price){return managerPtr->add_room(id, num, price);});
    manager_funcs.push_back([managerPtr](long id, long num, long price){return managerPtr->add_customer(id);});
    
    long t;
    long numTable = manager_funcs.size();

    printf("Initializing manager... ");
    fflush(stdout);

    numRelation = (long)params::instance()[P_RELATIONS];
    ids = (long*)malloc(numRelation * sizeof(long));
    for (i = 0; i < numRelation; i++) {
        ids[i] = i + 1;
    }

    for (t = 0; t < numTable; t++) {

        /* Shuffle ids */
        for (i = 0; i < numRelation; i++) {
            long x = randomPtr->random_generate() % numRelation;
            long y = randomPtr->random_generate() % numRelation;
            long tmp = ids[x];
            ids[x] = ids[y];
            ids[y] = tmp;
        }

        /* Populate table */
        for (i = 0; i < numRelation; i++) {
            bool status;
            long id = ids[i];
            long num = ((randomPtr->random_generate() % 5) + 1) * 100;
            long price = ((randomPtr->random_generate() % 5) * 10) + 50;
            status = manager_funcs[t](id, num, price);
            assert(status);
        }

    } /* for t */

    puts("done.");
    fflush(stdout);

    free(ids);

    return managerPtr;
}

static std::vector<Client_ptr>
initializeClients (Manager_ptr managerPtr)
{
  Random_ptr randPtr = std::make_shared<Random>();
    std::vector<Client_ptr> clients;
    
    long i;
    long numClient = (long)params::instance()[P_CLIENTS];
    long numTransaction = (long)params::instance()[P_TRANSACTIONS];
    long numTransactionPerClient;
    long numQueryPerTransaction = (long)params::instance()[P_NUMBER];
    long numRelation = (long)params::instance()[P_RELATIONS];
    long percentQuery = (long)params::instance()[P_QUERIES];
    long queryRange;
    long percentUser = (long)params::instance()[P_USER];
    long numThread = (long)params::instance()[P_THREADS];

    printf("Initializing clients... ");
    fflush(stdout);

   numTransactionPerClient = (long)((double)numTransaction / (double)numClient + 0.5);
    queryRange = (long)((double)percentQuery / 100.0 * (double)numRelation + 0.5);

    for (i = 0; i < numClient; i++) {
        clients.push_back( std::make_shared<Client>(i,
                                  managerPtr,
                                  numTransactionPerClient,
                                  numQueryPerTransaction,
                                  queryRange,
                                  percentUser));
    }

    puts("done.");
    printf("    Transactions        = %li\n", numTransaction);
    printf("    Clients             = %li\n", numClient);
    printf("    Transactions/client = %li\n", numTransactionPerClient);
    printf("    Queries/transaction = %li\n", numQueryPerTransaction);
    printf("    Relations           = %li\n", numRelation);
    printf("    Query percent       = %li\n", percentQuery);
    printf("    Query range         = %li\n", queryRange);
    printf("    Percent user        = %li\n", percentUser);
    printf("    Thread number       = %li\n", numThread);
    fflush(stdout);

    return clients;
}

void checkTables (Manager_ptr managerPtr)
{
    long i;
    long numRelation = (long)params::instance()[P_RELATIONS];
    std::map<long, Customer_ptr> & customerTablePtr = managerPtr->customer_table();
    
    printf("Checking tables... ");
    fflush(stdout);
    
    
    /* Check for unique customer IDs */
    long percentQuery = (long)params::instance()[P_QUERIES];
    long queryRange = (long)((double)percentQuery / 100.0 * (double)numRelation + 0.5);
    long maxCustomerId = queryRange + 1;
    for (i = 1; i <= maxCustomerId; i++) {
      if(customerTablePtr.find(i) != customerTablePtr.end())
	customerTablePtr.erase(i);
    }
    
    std::function<void (Manager::Reservations_t & ) > check_func=[numRelation](Manager::Reservations_t & r){
      for(int i = 1; i <= numRelation; i++)
      {
	if(r.find(i) != r.end())
	  r.erase(i);
      }
    };
    check_func(managerPtr->car_table());
    check_func(managerPtr->flight_table());
    check_func(managerPtr->room_table());

    puts("done.");
    fflush(stdout);
}





int main(int argc, char *argv[]){
    /* Initialization */
    if (params::parseArgs(argc, (char** const)argv) != 0)
      return 1;
    
    auto managerPtr = initializeManager();
    auto clients = initializeClients(managerPtr);
    int iThrdNum = params::instance()[P_THREADS];
    
      //setting thread number
  ff::rt::set_hardware_concurrency(iThrdNum);//Set concurrency
  ff::para<> a;
  a([]() {});
  ff::ff_wait(a);
  ff::scope_guard _l([](){}, [](){
      std::cout<<"writing records..."<<std::endl;
      RecordLocks::close_records();
      });
    
    /* Run transactions */
    printf("Running clients... ");
    fflush(stdout);
    
    std::string output_file = "times.json";
  boost::property_tree::ptree pt;
  pt.put("time-unit", "us");
  ff::scope_guard __t([](){}, [&pt](){boost::property_tree::write_json("time.json", pt);});
    
  std::function<void (const std::function<void ()> &)> time = [&pt](const std::function<void () > & f){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    f();
    end = std::chrono::system_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    pt.put("time", elapsed);
    std::cout<<"time: "<<elapsed <<" us"<<std::endl;
  };
  
#ifdef CACHE_EVAL
    /*Add papi to trace cache miss*/
    int EventSet,retVal;
    long long startRecords[2], endRecords[2];
    retVal = PAPI_library_init(PAPI_VER_CURRENT);
    assert(retVal == PAPI_VER_CURRENT);
    EventSet = PAPI_NULL;
    retVal = PAPI_create_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    //L2 TCM & TCA
    retVal = PAPI_add_event(EventSet, PAPI_L2_TCM);
    assert(retVal == PAPI_OK);
    retVal = PAPI_add_event(EventSet, PAPI_L2_TCA);
    assert(retVal == PAPI_OK);
    
    retVal = PAPI_start(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_read(EventSet, startRecords);
    assert(retVal == PAPI_OK);
    /*Add papi to trace cache miss*/
#endif
  
    //Parallel part
  time([&clients](){
    
    ff::paracontainer pg;
    for (Client_ptr p : clients)
    {
    std::cout<<"clients!"<<std::endl;
      ff::para<> para;
      para([&p](){Client::client_run(p);});
      pg.add(para);
    }
    ff::ff_wait(ff::all(pg));
  }
  );
    
#ifdef CACHE_EVAL
    /*Stop papi trace*/
    retVal = PAPI_stop(EventSet, endRecords);
    assert(retVal == PAPI_OK);
    retVal = PAPI_cleanup_eventset(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_destroy_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    PAPI_shutdown(); 
    //L2 result
    std::cout << "L2 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
    std::cout << "L2 total cache access = " << endRecords[1] - startRecords[1] << std::endl;
    /*Stop papi trace*/
#endif 
    puts("done.");
    checkTables(managerPtr);

    /* Clean up */
    printf("Deallocating memory... ");
    fflush(stdout);
    puts("done.");
    fflush(stdout);
    return 0;
}
