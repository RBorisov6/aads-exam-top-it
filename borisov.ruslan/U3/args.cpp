#include "args.hpp"
#include <cstring>

namespace
{
  const char IN_PREFIX[] = "in:";
  const char DATA_PREFIX[] = "data:";
}

borisov::Args borisov::parseArgs(const int argc, char **argv)
{
  borisov::Args result{"", nullptr, false, true};
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
      borisov::StrNode * const node = new borisov::StrNode{
          arg + (sizeof(DATA_PREFIX) - 1), result.dataFiles_};
      result.dataFiles_ = node;
    }
    else
    {
      result.valid_ = false;
      return result;
    }
  }
  if (result.dataFiles_ == nullptr)
  {
    result.valid_ = false;
  }
  return result;
}

void borisov::freeArgs(borisov::Args &args)
{
  borisov::StrNode *cur = args.dataFiles_;
  while (cur != nullptr)
  {
    borisov::StrNode * const next = cur->next_;
    delete cur;
    cur = next;
  }
  args.dataFiles_ = nullptr;
}
