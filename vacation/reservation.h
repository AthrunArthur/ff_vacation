#ifndef FF_BENCH_VACATION_RESERVATION_H_
#define FF_BENCH_VACATION_RESERVATION_H_
#include <memory>

enum OperationType {
    OPERATION_MAKE_RESERVATION = 0L,
    OPERATION_DELETE_CUSTOMER  = 1L,
    OPERATION_UPDATE_TABLE     = 2L,
    NUM_OPERATION
};

enum ActionType {
    ACTION_MAKE_RESERVATION = 0,
    ACTION_DELETE_CUSTOMER  = 1,
    ACTION_UPDATE_TABLES    = 2,
    NUM_ACTION
};

struct Reservation
{
  
  Reservation(long i, long p, long nt)
  : id(i)
  , price(p)
  , numTotal(nt)
  , numFree(nt)
  , numUsed(0){}
  
  bool		addToTotal(long num){
    numTotal += num;
    return true;
  }
  bool 		reserve()
  {
    numFree --;
    numUsed ++;
    return true;
  }
  bool		cancel()
  {
    numFree ++;
    numUsed --;
    return true;
  }
  
  bool		update_price(long np)
  {
    price = np;
    return true;
  }
  
  ulong		hash()
  {
    return (ulong)id;
  }
  static long	compare(Reservation *aPtr, Reservation *bPtr)
  {
    return (aPtr->id - bPtr->id);
  }
    long id;
    long numUsed;
    long numFree;
    long numTotal;
    long price;
};
typedef std::shared_ptr<Reservation> Reservation_ptr;

enum ReservationType {
    RESERVATION_CAR,
    RESERVATION_FLIGHT,
    RESERVATION_ROOM,
    NUM_RESERVATION_TYPE
};

struct ReservationInfo {
ReservationInfo(ReservationType t, long i, long p)
: type(t)
, id(i)
, price(p){}

static long compare(const ReservationInfo *aPtr, const ReservationInfo * bPtr)
{
  long typeDiff;

    typeDiff = aPtr->type - bPtr->type;

    return ((typeDiff != 0) ? (typeDiff) : (aPtr->id - bPtr->id));
}

    ReservationType type;
    long id;
    long price; /* holds price at time reservation was made */
};

typedef std::shared_ptr<ReservationInfo> ReservationInfo_ptr;

#endif