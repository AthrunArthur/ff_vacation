#include "client.h"
#include <array>
#include <cassert>
#include <vector>

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

void Client::make_reservation(Client_ptr clientPtr)
{
    // This function shall access car, flight, room and customer info!
    
    Manager_ptr managerPtr = clientPtr->managerPtr;
    Random_ptr  randomPtr  = clientPtr->randomPtr;
    long numQueryPerTransaction = clientPtr->numQueryPerTransaction;
    long queryRange             = clientPtr->queryRange;
    
    std::vector<long> types(numQueryPerTransaction);
    std::vector<long> ids(numQueryPerTransaction);
    long maxPrices[NUM_RESERVATION_TYPE] = {-1, -1, -1};
    long maxIds[NUM_RESERVATION_TYPE] = {-1, -1, -1};
    long n;
    long numQuery = randomPtr->random_generate() % numQueryPerTransaction + 1;
    long customerId = randomPtr->random_generate() % queryRange + 1;
    for (n = 0; n < numQuery; n ++){
        types[n] = randomPtr->random_generate()%NUM_RESERVATION_TYPE;
        ids[n] = (randomPtr->random_generate() % queryRange) + 1;
    }
    bool isFound = false;
    for(n = 0; n < numQuery; n ++){
        long t = types[n];
        long id = ids[n];
        long price = -1;
        switch (t) {
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
        if (price > maxPrices[t]) {
            maxPrices[t] = price;
            maxIds[t] = id;
            isFound = true;
        }
    }
    if (isFound) {
        managerPtr->add_customer(customerId);
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
    long bill = managerPtr->query_customer_bill(customerId);
    if ( bill >= 0){
        managerPtr->del_customer(customerId);
    }
}

void Client::action_update_label(std::shared_ptr<Client> clientPtr)
{
    Random_ptr  randomPtr  = clientPtr->randomPtr;
    Manager_ptr managerPtr = clientPtr->managerPtr;
    long numQueryPerTransaction = clientPtr->numQueryPerTransaction;
    long numUpdate = randomPtr->random_generate() % numQueryPerTransaction + 1;
    long queryRange             = clientPtr->queryRange;
    
    std::vector<long> types(numQueryPerTransaction);
    std::vector<long> ids(numQueryPerTransaction);
    std::vector<long> ops(numQueryPerTransaction);
    std::vector<long> prices(numQueryPerTransaction);
    long n;
    for(n = 0; n < numUpdate; n++)
    {
        types[n] = randomPtr->random_generate() % NUM_RESERVATION_TYPE;
        ids[n] = (randomPtr->random_generate() % queryRange) + 1;
        ops[n] = randomPtr->random_generate() % 2;
        if(ops[n]){
            prices[n] = ((randomPtr->random_generate() % 5)*10) + 50;
        }
    }
    
    for (n = 0; n < numUpdate; n++) {
        long t = types[n];
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
void Client::client_run(  Client_ptr   clientPtr)
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

    //parallel part
    for (i = 0; i < numOperation; i++) {

	long r = randomPtr->random_generate() % 100;
	ActionType action = selectAction(r, percentUser);

        switch (action) {

            case ACTION_MAKE_RESERVATION: {
                make_reservation(clientPtr);
                break;
            }

            case ACTION_DELETE_CUSTOMER: {
                action_del_customer(clientPtr);
                break;
            }

            case ACTION_UPDATE_TABLES: {
                action_update_label(clientPtr);
                break;
            }

            default:
                assert(0);

        } /* switch (action) */

    } /* for i */

    //TM_THREAD_EXIT();
}

