#include "args.hpp"
#include <cstring>

namespace
{
  const char IN_PREFIX[] = "in:";
  const char DATA_PREFIX[] = "data:";
  const int MAX_ARGC = 3;
}

borisov::Args borisov::parseArgs(const int argc, char **argv)
{
  borisov::Args result{"", "", false, true};
  if (argc > MAX_ARGC)
  {
    result.valid_ = false;
    return result;
  }
  for (int i = 1; i < argc; ++i)
  {
    const char * const arg = argv[i];
    if (std::strncmp(arg, IN_PREFIX, sizeof(IN_PREFIX) - 1) == 0)
    {
      if (result.inSet_)
      {
        result.valid_ = false;
        return result;
      }
      result.inFile_ = arg + (sizeof(IN_PREFIX) - 1);
      result.inSet_ = true;
    }
    else if (std::strncmp(arg, DATA_PREFIX, sizeof(DATA_PREFIX) - 1) == 0)
    {
      if (!result.dataFile_.empty())
      {
        result.valid_ = false;
        return result;
      }
      result.dataFile_ = arg + (sizeof(DATA_PREFIX) - 1);
    }
    else
    {
      result.valid_ = false;
      return result;
    }
  }
  if (result.dataFile_.empty())
  {
    result.valid_ = false;
  }
  return result;
}
