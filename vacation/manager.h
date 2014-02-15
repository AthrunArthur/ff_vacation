#ifndef FF_BENCH_VACATION_MANAGER_H_
#define FF_BENCH_VACATION_MANAGER_H_

#include "reservation.h"
#include "args.h"
#include "customer.h"

#include <map>


class Manager{
public:
  
  typedef std::map<long, Reservation_ptr> Reservations_t;
  
  Manager(){}
  
  bool add_reservation(Reservations_t & table, long id, long num, long price);
  
  bool add_car(long carId, long numCars, long price)
  {
    return add_reservation(m_car_table, carId, numCars, price);
  }
  bool del_car(long carId, long numCar)
  {
    return add_reservation(m_car_table, carId, -numCar, -1);
  }
  bool add_room(long roomId, long numRooms, long price)
  {
    return add_reservation(m_room_table, roomId, numRooms, price);
  }
  bool del_room(long roomId, long numRooms)
  {
    return add_reservation(m_room_table, roomId, -numRooms, -1);
  }
  bool add_flight(long flightId, long numFlights, long price)
  {
    return add_reservation(m_flight_table, flightId, numFlights, price);
  }
  bool del_flight(long flightId);
  
  bool add_customer(long customerId);
  bool del_customer(long customerId);
  
  Reservations_t & car_table(){return m_car_table;}
  Reservations_t & room_table(){return m_room_table;}
  Reservations_t & flight_table(){return m_flight_table;}
  std::map<long, Customer_ptr> &  customer_table(){return m_customer_table;}
  
  long query_car(long id){return query_free(m_car_table, id);}
  long query_car_price(long id){return query_price(m_car_table, id);}
  
  long query_room(long id){return query_free(m_room_table, id);}
  long query_room_price(long id){return query_price(m_room_table, id);}
  long query_flight(long id){return query_free(m_flight_table, id);}
  long query_flight_price(long id){return query_price(m_flight_table, id);}
  
  long query_customer_bill(long id);
  
  bool reserve_car(long customerId, long carId){
    return reserve(m_car_table,
                   m_customer_table,
                   customerId,
                   carId,
                   RESERVATION_CAR);
  }
  
  bool reserve_room(long customerId, long roomId){
    return reserve(m_room_table,
                   m_customer_table,
                   customerId,
                   roomId,
                   RESERVATION_ROOM);
  }
  bool reserve_flight(long customerId, long flightId){
    return reserve(m_flight_table,
                   m_customer_table,
                   customerId,
                   flightId,
                   RESERVATION_FLIGHT);
  }
  
  bool cancel_car(long customerId, long carId){
    return cancel(m_car_table,
                   m_customer_table,
                   customerId,
                   carId,
                   RESERVATION_CAR);
  }
  
  bool cancel_room(long customerId, long roomId){
    return cancel(m_room_table,
                   m_customer_table,
                   customerId,
                   roomId,
                   RESERVATION_ROOM);
  }
  bool cancel_flight(long customerId, long flightId){
    return cancel(m_flight_table,
                   m_customer_table,
                   customerId,
                   flightId,
                   RESERVATION_FLIGHT);
  }
  
  
protected:
  
  long query_free(Reservations_t &table, long id);
  
  long query_price(Reservations_t &table, long id);
  
  bool  reserve (Reservations_t & table, std::map<long, Customer_ptr>& customer_table,
         long customerId, long id, ReservationType type);
  
  bool 	cancel(Reservations_t & table, std::map<long, Customer_ptr> & customer_table, 
    long customerId, long id, ReservationType type);
  
  
protected:
  Reservations_t m_car_table;
  Reservations_t m_room_table;
  Reservations_t m_flight_table;
  std::map<long, Customer_ptr> m_customer_table;
};

typedef std::shared_ptr<Manager> Manager_ptr;

#endif