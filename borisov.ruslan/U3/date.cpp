#include "date.hpp"

int borisov::dateCmp(const borisov::Date a, const borisov::Date b)
{
  if (a.year_ != b.year_)
  {
    return a.year_ < b.year_ ? -1 : 1;
  }
  if (a.month_ != b.month_)
  {
    return a.month_ < b.month_ ? -1 : 1;
  }
  if (a.day_ != b.day_)
  {
    return a.day_ < b.day_ ? -1 : 1;
  }
  return 0;
}
