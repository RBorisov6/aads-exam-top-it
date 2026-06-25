#ifndef BORISOV_U2_ARGS_HPP
#define BORISOV_U2_ARGS_HPP
#include <string>

namespace borisov
{
  struct Args
  {
    std::string inFile_;
    std::string dataFile_;
    bool inSet_;
    bool valid_;
  };

  Args parseArgs(int argc, char **argv);
}

#endif
