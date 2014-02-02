#ifndef FF_BENCH_VACATION_CUSTOMER_H_
#define FF_BENCH_VACATION_CUSTOMER_H_

#include "reservation.h"
#include <list>
#include <memory>
#include <boost/iterator/iterator_concepts.hpp>
struct Customer
{
  Customer(long i)
  : id(i){}
  
  static long		compare(Customer *aPtr, Customer *bPtr)
  {
    return aPtr->id - bPtr->id;
  }
  
  bool			add_reservation(ReservationType type, long id, long price);
  
  bool			remove_reservation(ReservationType type, long id);
  
  long			get_bill() const;
  
  long id;
  std::list<ReservationInfo_ptr> reservationInfoList;
  
};
typedef std::shared_ptr<Customer> Customer_ptr;

#endif