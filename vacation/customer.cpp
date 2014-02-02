#include "customer.h"

bool Customer::add_reservation(ReservationType type, long int id, long int price)
{
  ReservationInfo_ptr p = std::make_shared<ReservationInfo>(type, id, price);
  reservationInfoList.push_back(p);
  return true;
}

bool Customer::remove_reservation(ReservationType type, long int id)
{
    reservationInfoList.remove_if(
      [type, id](const ReservationInfo_ptr & p)
      {return p->id== id && p->type == type;}
    );
    return true;
}

long int Customer::get_bill() const
{
  long bill = 0;
  for(const ReservationInfo_ptr & p: reservationInfoList)
  {
    bill += p->price;
  }
  return bill;
}

