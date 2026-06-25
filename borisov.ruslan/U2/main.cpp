#include <iostream>
#include <fstream>
#include <string>
#include "args.hpp"
#include "commands.hpp"
#include <person.hpp>
#include <meeting.hpp>

namespace
{
  bool skipSpace(const std::string &line, size_t &pos)
  {
    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
    {
      ++pos;
    }
    return pos < line.size();
  }

  bool parseUnsigned(const std::string &line, size_t &pos, size_t &val)
  {
    if (!skipSpace(line, pos))
    {
      return false;
    }
    if (line[pos] < '0' || line[pos] > '9')
    {
      return false;
    }
    val = 0;
    while (pos < line.size() && line[pos] >= '0' && line[pos] <= '9')
    {
      val = val * 10 + static_cast< size_t >(line[pos] - '0');
      ++pos;
    }
    return true;
  }

  bool parseWord(const std::string &line, size_t &pos, std::string &val)
  {
    if (!skipSpace(line, pos))
    {
      return false;
    }
    const size_t start = pos;
    while (pos < line.size() && line[pos] != ' ' && line[pos] != '\t')
    {
      ++pos;
    }
    val = line.substr(start, pos - start);
    return true;
  }

  bool parseQuoted(const std::string &line, size_t &pos, std::string &val)
  {
    if (!skipSpace(line, pos))
    {
      return false;
    }
    if (line[pos] != '"')
    {
      return false;
    }
    ++pos;
    const size_t start = pos;
    while (pos < line.size() && line[pos] != '"')
    {
      ++pos;
    }
    if (pos >= line.size())
    {
      return false;
    }
    val = line.substr(start, pos - start);
    ++pos;
    return true;
  }

  bool processCommand(const std::string &line, borisov::PersonNode **persons,
      borisov::MeetingNode **meetings)
  {
    size_t pos = 0;
    std::string cmd;
    if (!parseWord(line, pos, cmd))
    {
      return false;
    }
    if (cmd == "anons")
    {
      return borisov::cmdAnons(std::cout, *persons, *meetings);
    }
    if (cmd == "deanon")
    {
      size_t anonId = 0;
      size_t id = 0;
      if (!parseUnsigned(line, pos, anonId) || !parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdDeanon(meetings, *persons, anonId, id);
    }
    if (cmd == "redesc")
    {
      size_t id = 0;
      std::string desc;
      if (!parseUnsigned(line, pos, id) || !parseQuoted(line, pos, desc))
      {
        return false;
      }
      return borisov::cmdRedesc(persons, *meetings, id, desc);
    }
    if (cmd == "desc")
    {
      size_t id = 0;
      if (!parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdDesc(std::cout, *persons, *meetings, id);
    }
    if (cmd == "meets")
    {
      size_t id = 0;
      if (!parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdMeets(std::cout, *persons, *meetings, id);
    }
    if (cmd == "commons")
    {
      size_t id1 = 0;
      size_t id2 = 0;
      if (!parseUnsigned(line, pos, id1) || !parseUnsigned(line, pos, id2))
      {
        return false;
      }
      return borisov::cmdCommons(std::cout, *persons, *meetings, id1, id2);
    }
    if (cmd == "less")
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseUnsigned(line, pos, time) || !parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdLess(std::cout, *persons, *meetings, time, id);
    }
    if (cmd == "greater")
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseUnsigned(line, pos, time) || !parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdGreater(std::cout, *persons, *meetings, time, id);
    }
    if (cmd == "out-persons")
    {
      std::string filename;
      if (!parseWord(line, pos, filename))
      {
        return false;
      }
      return borisov::cmdOutPersons(filename, *persons);
    }
    return false;
  }
}

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
    std::cerr << "bad args\n";
    return 1;
  }
  borisov::PersonNode *persons = nullptr;
  if (args.inSet_)
  {
    std::ifstream inFile(args.inFile_.c_str());
    if (!inFile.is_open())
    {
      std::cerr << "cannot open file\n";
      return 2;
    }
    size_t count = 0;
    size_t skipped = 0;
    bool hasInput = false;
    try
    {
      persons = borisov::readPersons(inFile, count, skipped, hasInput);
    }
    catch (...)
    {
      std::cerr << "read error\n";
      return 2;
    }
  }
  borisov::MeetingNode *meetings = nullptr;
  {
    std::ifstream dataFile(args.dataFile_.c_str());
    if (!dataFile.is_open())
    {
      borisov::freePersons(persons);
      std::cerr << "cannot open file\n";
      return 2;
    }
    try
    {
      meetings = borisov::readMeetings(dataFile);
    }
    catch (...)
    {
      borisov::freePersons(persons);
      std::cerr << "invalid meeting data\n";
      return 3;
    }
  }
  std::string line;
  while (std::getline(std::cin, line))
  {
    if (!processCommand(line, &persons, &meetings))
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  borisov::freePersons(persons);
  borisov::freeMeetings(meetings);
  return 0;
}
