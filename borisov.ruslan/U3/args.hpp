#ifndef BORISOV_U3_ARGS_HPP
#define BORISOV_U3_ARGS_HPP
#include <string>

namespace borisov
{
  struct StrNode
  {
    std::string data_;
    StrNode *next_;
  };

  struct Args
  {
    std::string inFile_;
    StrNode *dataFiles_;
    bool inSet_;
    bool valid_;
  };

  Args parseArgs(int argc, char **argv);
  void freeArgs(Args &args);
}

#endif
