#ifndef BORISOV_U3_DATE_HPP
#define BORISOV_U3_DATE_HPP
#include <cstddef>

namespace borisov
{
  struct Date
  {
    size_t day_;
    size_t month_;
    size_t year_;
  };

  int dateCmp(Date a, Date b);
}

#endif
