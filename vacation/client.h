#ifndef FF_BENCH_VACATION_CLIENT_H_
#define FF_BENCH_VACATION_CLIENT_H_
#include "customer.h"
#include "manager.h"
#include "reservation.h"
#include "random.h"


class Client
{
public:
  Client(long pid,
              Manager_ptr pmanagerPtr,
              long pnumOperation,
              long pnumQueryPerTransaction,
              long pqueryRange,
              long ppercentUser)
  :id(pid)
  , managerPtr(pmanagerPtr)
  , randomPtr(std::make_shared<Random>())
  , numOperation(pnumOperation)
  , numQueryPerTransaction(pnumQueryPerTransaction)
  , queryRange(pqueryRange)
  , percentUser(ppercentUser)
  {
    randomPtr->random_seed(id);
  }
  
  
  static ActionType selectAction(long r, long percentUser);
  
  static void client_run( std::shared_ptr<Client>  client);

    long id;
    Manager_ptr managerPtr;
    Random_ptr randomPtr;
    long numOperation;
    long numQueryPerTransaction;
    long queryRange;
    long percentUser;
    
protected:
    static void make_reservation(std::shared_ptr<Client>  clientPtr, long rt);
    static void action_del_customer(std::shared_ptr<Client> clientPtr);
    static void action_update_label(std::shared_ptr<Client> clientPtr, long rt);
    
};

typedef std::shared_ptr<Client> Client_ptr;

#endif