#include "client.h"
#include <array>
#include <cassert>
#include <vector>
#include <iostream>
#include <ff.h>
#include "args.h"
#define RECORD_LOCKS
#ifdef RECORD_LOCKS
#include "record.h"
#endif

#define GRANULARIYT 10000
#define UPDATE_MAX 10000
#define DEL_CUSTOMER_MAX 1000

ActionType Client::selectAction(long int r, long int percentUser)
{
    ActionType action;

    if (r < percentUser) {
        action = ActionType::ACTION_MAKE_RESERVATION;
    } else if (r & 1) {
        action = ActionType::ACTION_DELETE_CUSTOMER;
    } else {
        action = ActionType::ACTION_UPDATE_TABLES;
    }
    return action;
}

void Client::make_reservation(Client_ptr clientPtr, long rt)
{
    // This function shall access car, flight, room and customer info!
    
    Manager_ptr managerPtr = clientPtr->managerPtr;
    Random_ptr  randomPtr  = clientPtr->randomPtr;
    long numQueryPerTransaction = clientPtr->numQueryPerTransaction;
    long queryRange             = clientPtr->queryRange;
    
    std::vector<long> ids(numQueryPerTransaction);
    long maxPrices[NUM_RESERVATION_TYPE] = {-1, -1, -1};
    long maxIds[NUM_RESERVATION_TYPE] = {-1, -1, -1};
    long n;
    long numQuery = randomPtr->random_generate() % numQueryPerTransaction + 1;
    long customerId = randomPtr->random_generate() % queryRange + 1;
    for (n = 0; n < numQuery; n ++){
        ids[n] = (randomPtr->random_generate() % queryRange) + 1;
    }
    
    bool isFound = false;
    //std::cout<<"make_reservation numQuery: "<<numQuery<<std::endl;
    for(n = 0; n < numQuery; n ++){
        long id = ids[n];
        long price = -1;
        switch (rt) {
            case RESERVATION_CAR:
                if(managerPtr->query_car(id) >=0){
                    price = managerPtr->query_car_price(id);
                }
                break;
            case RESERVATION_FLIGHT:
                if(managerPtr->query_flight(id) >=0){
                    price = managerPtr->query_flight_price(id);
                }
                break;
            case RESERVATION_ROOM:
                if(managerPtr->query_room(id) >= 0)
                {
                    price = managerPtr->query_room_price(id);
                }
                break;
            default:
                assert(0);
        }
        if (price > maxPrices[rt]) {
            maxPrices[rt] = price;
            maxIds[rt] = id;
            isFound = true;
        }
    }
    if (isFound) {
      managerPtr->customer_lock().lock();
#ifdef RECORD_LOCKS
        RecordLocks::record(&(managerPtr->customer_lock()));
#endif
        managerPtr->add_customer(customerId);
	managerPtr->customer_lock().unlock();
    }
    if (maxIds[RESERVATION_CAR] > 0) {
        managerPtr->reserve_car(customerId, maxIds[RESERVATION_CAR]);
    }
    if (maxIds[RESERVATION_FLIGHT] > 0) {
        managerPtr->reserve_flight(customerId, maxIds[RESERVATION_FLIGHT]);
    }
    if (maxIds[RESERVATION_ROOM] > 0) {
        managerPtr->reserve_room(customerId, maxIds[RESERVATION_ROOM]);
    }
}

void Client::action_del_customer(std::shared_ptr<Client> clientPtr)
{
  
    Random_ptr  randomPtr  = clientPtr->randomPtr;
    Manager_ptr managerPtr = clientPtr->managerPtr;
    long queryRange             = clientPtr->queryRange;
    long customerId = randomPtr->random_generate() % queryRange + 1;
    
    managerPtr->car_lock().lock();
    managerPtr->room_lock().lock();
    managerPtr->flight_lock().lock();
    managerPtr->customer_lock().lock();
#ifdef RECORD_LOCKS
    RecordLocks::record(&(managerPtr->car_lock()));
    RecordLocks::record(&(managerPtr->room_lock()));
    RecordLocks::record(&(managerPtr->flight_lock()));
    RecordLocks::record(&(managerPtr->customer_lock()));
#endif
    
    long bill = managerPtr->query_customer_bill(customerId);
    if ( bill >= 0){
        managerPtr->del_customer(customerId);
    }
    
    managerPtr->customer_lock().unlock();
    managerPtr->flight_lock().unlock();
    managerPtr->room_lock().unlock();
    managerPtr->car_lock().unlock();
}

void Client::action_update_label(std::shared_ptr<Client> clientPtr, long rt)
{
    Random_ptr  randomPtr  = clientPtr->randomPtr;
    Manager_ptr managerPtr = clientPtr->managerPtr;
    long numQueryPerTransaction = clientPtr->numQueryPerTransaction;
    long numUpdate = randomPtr->random_generate() % numQueryPerTransaction + 1;
    long queryRange             = clientPtr->queryRange;
    
    std::vector<long> ids(numQueryPerTransaction);
    std::vector<long> ops(numQueryPerTransaction);
    std::vector<long> prices(numQueryPerTransaction);
    long n;
    for(n = 0; n < numUpdate; n++)
    {
        ids[n] = (randomPtr->random_generate() % queryRange) + 1;
        ops[n] = randomPtr->random_generate() % 2;
        if(ops[n]){
            prices[n] = ((randomPtr->random_generate() % 5)*10) + 50;
        }
    }
    
    for (n = 0; n < numUpdate; n++) {
        long t = rt;
        long id = ids[n];
        long doAdd = ops[n];
        if (doAdd) {
            long newPrice = prices[n];
            switch (t) {
                case RESERVATION_CAR:
                    managerPtr->add_car(id, 100, newPrice);
                    break;
                case RESERVATION_FLIGHT:
                    managerPtr->add_flight(id, 100, newPrice);
                    break;
                case RESERVATION_ROOM:
                    managerPtr->add_room(id, 100, newPrice);
                    break;
                default:
                    assert(0);
            }
        } else { /* do delete */
            switch (t) {
                case RESERVATION_CAR:
                    managerPtr->del_car(id, 100);
                    break;
                case RESERVATION_FLIGHT:
                    managerPtr->del_flight(id);
                    break;
                case RESERVATION_ROOM:
                    managerPtr->del_room(id, 100);
                    break;
                default:
                    assert(0);
            }
        }
    }
    
}
void Client::client_run(Client_ptr   clientPtr)
{
    //Client_ptr clientPtr = clients[thrd_id];
    
    Manager_ptr managerPtr = clientPtr->managerPtr;
    Random_ptr  randomPtr  = clientPtr->randomPtr;

    long numOperation           = clientPtr->numOperation;
    long numQueryPerTransaction = clientPtr->numQueryPerTransaction;
    long queryRange             = clientPtr->queryRange;
    long percentUser            = clientPtr->percentUser;

    std::vector<long> types(numQueryPerTransaction);
    std::vector<long> ids(numQueryPerTransaction);
    std::vector<long> ops(numQueryPerTransaction);
    std::vector<long> prices(numQueryPerTransaction);
    
    long i;
    long  nums[NUM_RESERVATION_TYPE * NUM_RESERVATION_TYPE]{0};
    
    
    std::cout<<"client_run numOperation : "<<numOperation<<std::endl;
    //parallel part
    ff::paragroup pg;
    bool using_ff_lock = (params::instance()[P_FF_LOCK] == 1);
    
    for (i = 0; i < numOperation; i++) {

	long r = randomPtr->random_generate() % 100;
	ActionType action = selectAction(r, percentUser);
	long rt = randomPtr->random_generate() % NUM_RESERVATION_TYPE;

	nums[action * NUM_RESERVATION_TYPE + rt] ++;
	long & tn = nums[action * NUM_RESERVATION_TYPE + rt];
	ff::mutex * pMutex = nullptr;
	switch(rt){
	  case     RESERVATION_CAR:
	    pMutex = & managerPtr->car_lock();
	    break;
	  case  RESERVATION_FLIGHT:
	    pMutex = & managerPtr->flight_lock();
	    break;
	  case  RESERVATION_ROOM:
	    pMutex = & managerPtr->room_lock();
	    break;
	}
	ff::mutex_id_t mutexID = ff::invalid_mutex_id;
	if(using_ff_lock)
	  mutexID = pMutex->id();
        switch (action) {
            case ACTION_MAKE_RESERVATION: {
	      if (tn >= GRANULARIYT)
	      {
		//std::cout<<"mutexID "<<reinterpret_cast<uint64_t>(mutexID)<<std::endl;
		ff::para<> p;
		p([tn, rt, pMutex, clientPtr](){
		  pMutex->lock();
#ifdef RECORD_LOCKS
                  RecordLocks::record(pMutex);
#endif
		  for(int i = 0; i < tn; i ++)
		  {
		    make_reservation(clientPtr, rt);
		  }
		  pMutex->unlock();
		}, mutexID);
		pg.add(p);
		tn = 0;
	      }
                break;
            }

            case ACTION_DELETE_CUSTOMER: {
	      if(tn >= DEL_CUSTOMER_MAX){
		ff::para<> p;
		p([tn, clientPtr](){
		  for(int i = 0; i < tn; i++)
		  {
		    action_del_customer(clientPtr);
		  }
		});
		pg.add(p);
		tn = 0;
	      }
                break;
            }

            case ACTION_UPDATE_TABLES: {
	      if(tn >= UPDATE_MAX){
		ff::para<> p;
		p([tn, rt, pMutex, clientPtr](){
		  pMutex->lock();
#ifdef RECORD_LOCKS
                  RecordLocks::record(pMutex);
#endif
		  for(int i = 0; i < tn; ++i)
		  {
		      action_update_label(clientPtr, rt);
		  }
		  pMutex->unlock();
		}, mutexID);
		pg.add(p);
		tn = 0;
	      }
                break;
            }

            default:
                assert(0);

        } /* switch (action) */

    } /* for i */
    ff::ff_wait(ff::all(pg));

    //TM_THREAD_EXIT();
}

