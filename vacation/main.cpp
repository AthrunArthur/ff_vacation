#include "args.h"
#include "client.h"
#include "customer.h"
#include "manager.h"
#include <functional>
#include <cassert>
#include <stdio.h>

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
    
    
    /* Run transactions */
    printf("Running clients... ");
    fflush(stdout);
    
    //Parallel part
    for (Client_ptr p : clients)
    {
      Client::client_run(p);
    }
    puts("done.");
    checkTables(managerPtr);

    /* Clean up */
    printf("Deallocating memory... ");
    fflush(stdout);
    puts("done.");
    fflush(stdout);
    return 0;
}