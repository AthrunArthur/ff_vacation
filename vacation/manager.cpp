#include "manager.h"

bool Manager::add_reservation(Manager::Reservations_t& table, long int id, long int num, long int price)
{
  auto res = table.find(id);
  if(res == table.end())
  {
    Reservation_ptr r = std::make_shared<Reservation>(id, num, price);
    table.insert(std::make_pair(id, r));
    return true;
  }
  else{
    Reservation_ptr r = res->second;
    r->addToTotal(num);
    if(r->numTotal == 0)
      table.erase(id);
  }
  return true; 
}

bool Manager::del_flight(long int flightId)
{
  auto res = m_flight_table.find(flightId);
  if(res == m_flight_table.end())
    return false;
  if(res->second->numUsed > 0)
    return false;
  auto t = res->second->numTotal;
  return add_reservation(m_flight_table, flightId, -t, -1);
}

bool Manager::add_customer(long int customerId)
{
  auto res = m_customer_table.find(customerId);
  if(res != m_customer_table.end())
    return false;
  Customer_ptr cp = std::make_shared<Customer>(customerId);
  m_customer_table.insert(std::make_pair(customerId, cp));
  return true;
}

bool Manager::del_customer(long int customerId)
{
    Manager::Reservations_t* reservationTables[NUM_RESERVATION_TYPE];
    bool status;

    if(m_customer_table.find(customerId) == m_customer_table.end())
      return false;
    
    Customer_ptr customerPtr = m_customer_table[customerId];
    

    reservationTables[RESERVATION_CAR] = &m_car_table;
    reservationTables[RESERVATION_ROOM] = &m_room_table;
    reservationTables[RESERVATION_FLIGHT] = & m_flight_table;

    /* Cancel this customer's reservations */
    std::list<ReservationInfo_ptr> & reservationInfoList = customerPtr->reservationInfoList;
    
    for (ReservationInfo_ptr r: reservationInfoList)
    {
      auto it = reservationTables[r->type]->find(r->id);
      if(it == reservationTables[r->type]->end())
	continue;
      Reservation_ptr rp = it->second;
      rp->cancel();
    }
    reservationInfoList.clear();
    m_customer_table.erase(customerId);

    return true;
}

long int Manager::query_free(Manager::Reservations_t& table, long id)
{
    long numFree = -1;
    auto it = table.find(id);
    if(it == table.end())
      return numFree;
    numFree = it->second->numFree;

    return numFree;
}

long int Manager::query_price(Manager::Reservations_t& table, long int id)
{
    long price = -1;
    auto it = table.find(id);
    if(it == table.end())
      return price;
    price = it->second->price;

    return price;
}

long int Manager::query_customer_bill(long int id)
{
  long bill = -1;
  auto it = m_customer_table.find(id);
  if(it == m_customer_table.end())
    return bill;
  return it->second->get_bill();
}

bool Manager::reserve(Manager::Reservations_t& table, std::map< long int, Customer_ptr >& customer_table, long int customerId, long int id, ReservationType type)
{
    auto cit = customer_table.find(customerId);
    if(cit == customer_table.end())
      return false;
    
    auto rit = table.find(id);
    if(rit == table.end())
      return false;
    
    rit->second->reserve();
    
    
    cit->second->add_reservation(type, id, rit->second->price);
    return true;
}

bool Manager::cancel(Manager::Reservations_t& table, std::map< long int, Customer_ptr >& customer_table, long int customerId, long int id, ReservationType type)
{
    auto cit = customer_table.find(customerId);
    if(cit == customer_table.end())
      return false;

    auto rit = table.find(id);
    if(rit == table.end())
      return false;
    rit->second->cancel();
    cit->second->remove_reservation(type, id);
    return true;
}

