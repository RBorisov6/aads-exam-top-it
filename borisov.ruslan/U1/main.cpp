#include <iostream>
#include <fstream>
#include "args.hpp"
#include <person.hpp>

int main(int argc, char **argv)
{
  if (argc > 3)
  {
    std::cerr << "bad args\n";
    return 0;
  }
  const borisov::Args args = borisov::parseArgs(argc, argv);
  if (!args.valid_)
  {
    return 1;
  }
  std::ifstream inFstream;
  if (args.inSet_)
  {
    inFstream.open(args.inFile_);
    if (!inFstream.is_open())
    {
      return 2;
    }
  }
  std::istream &in = args.inSet_ ? static_cast< std::istream & >(inFstream) : std::cin;
  size_t count = 0;
  size_t skipped = 0;
  bool hasInput = false;
  borisov::PersonNode * const head = borisov::readPersons(in, count, skipped, hasInput);
  if (args.inSet_)
  {
    inFstream.close();
  }
  std::ofstream outFstream;
  if (args.outSet_)
  {
    outFstream.open(args.outFile_);
    if (!outFstream.is_open())
    {
      borisov::freePersons(head);
      return 2;
    }
  }
  std::ostream &out = args.outSet_ ? static_cast< std::ostream & >(outFstream) : std::cout;
  const bool empty = (head == nullptr);
  borisov::writePersons(out, head);
  borisov::freePersons(head);
  if (empty)
  {
    out << "\n";
  }
  std::cerr << count << " " << skipped << "\n";
  return 0;
}
