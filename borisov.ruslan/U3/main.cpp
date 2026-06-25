#include <iostream>
#include <fstream>
#include <string>
#include "args.hpp"
#include "commands.hpp"
#include "meeting.hpp"
#include <person.hpp>

namespace
{
  struct RangeNode
  {
    borisov::Date start_;
    borisov::Date end_;
    bool empty_;
    RangeNode *prev_;
  };

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

  borisov::DatedMeetingNode *appendMeetings(borisov::DatedMeetingNode * const head,
      borisov::DatedMeetingNode * const extra)
  {
    if (head == nullptr)
    {
      return extra;
    }
    borisov::DatedMeetingNode *cur = head;
    while (cur->next_ != nullptr)
    {
      cur = cur->next_;
    }
    cur->next_ = extra;
    return head;
  }

  void computeRange(const borisov::DatedMeetingNode * const meetings,
      borisov::Date &minDate, borisov::Date &maxDate, bool &hasDate)
  {
    hasDate = false;
    const borisov::DatedMeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      if (!hasDate)
      {
        minDate = cur->data_.date_;
        maxDate = cur->data_.date_;
        hasDate = true;
      }
      else
      {
        if (borisov::dateCmp(cur->data_.date_, minDate) < 0)
        {
          minDate = cur->data_.date_;
        }
        if (borisov::dateCmp(cur->data_.date_, maxDate) > 0)
        {
          maxDate = cur->data_.date_;
        }
      }
      cur = cur->next_;
    }
  }

  bool cmdRange(std::ostream &out, const RangeNode * const rangeStack,
      const bool hasDate)
  {
    if (!hasDate || rangeStack == nullptr || rangeStack->empty_)
    {
      out << "<EMPTY>\n";
      return true;
    }
    const borisov::Date &s = rangeStack->start_;
    const borisov::Date &e = rangeStack->end_;
    out << s.day_ << " " << s.month_ << " " << s.year_
        << " : " << e.day_ << " " << e.month_ << " " << e.year_ << "\n";
    return true;
  }

  bool cmdAfter(RangeNode **rangeStack,
      const borisov::DatedMeetingNode * const meetings, const borisov::Date date)
  {
    if (*rangeStack == nullptr || (*rangeStack)->empty_)
    {
      return false;
    }
    const borisov::Date curEnd = (*rangeStack)->end_;
    borisov::Date newStart{0, 0, 0};
    bool found = false;
    const borisov::DatedMeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      const borisov::Date d = cur->data_.date_;
      if (borisov::dateCmp(d, date) >= 0 && borisov::dateCmp(d, curEnd) <= 0)
      {
        if (!found || borisov::dateCmp(d, newStart) < 0)
        {
          newStart = d;
          found = true;
        }
      }
      cur = cur->next_;
    }
    const borisov::Date dummy{0, 0, 0};
    if (!found)
    {
      RangeNode * const node = new RangeNode{dummy, dummy, true, *rangeStack};
      *rangeStack = node;
      return true;
    }
    RangeNode * const node = new RangeNode{newStart, curEnd, false, *rangeStack};
    *rangeStack = node;
    return true;
  }

  bool cmdBefore(RangeNode **rangeStack,
      const borisov::DatedMeetingNode * const meetings, const borisov::Date date)
  {
    if (*rangeStack == nullptr || (*rangeStack)->empty_)
    {
      return false;
    }
    const borisov::Date curStart = (*rangeStack)->start_;
    borisov::Date newEnd{0, 0, 0};
    bool found = false;
    const borisov::DatedMeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      const borisov::Date d = cur->data_.date_;
      if (borisov::dateCmp(d, date) <= 0 && borisov::dateCmp(d, curStart) >= 0)
      {
        if (!found || borisov::dateCmp(d, newEnd) > 0)
        {
          newEnd = d;
          found = true;
        }
      }
      cur = cur->next_;
    }
    const borisov::Date dummy{0, 0, 0};
    if (!found)
    {
      RangeNode * const node = new RangeNode{dummy, dummy, true, *rangeStack};
      *rangeStack = node;
      return true;
    }
    RangeNode * const node = new RangeNode{curStart, newEnd, false, *rangeStack};
    *rangeStack = node;
    return true;
  }

  bool cmdPopRange(RangeNode **rangeStack)
  {
    if (*rangeStack == nullptr || (*rangeStack)->prev_ == nullptr)
    {
      return false;
    }
    RangeNode * const top = *rangeStack;
    *rangeStack = top->prev_;
    delete top;
    return true;
  }

  void freeRangeStack(RangeNode *rangeStack)
  {
    while (rangeStack != nullptr)
    {
      RangeNode * const prev = rangeStack->prev_;
      delete rangeStack;
      rangeStack = prev;
    }
  }

  bool processCommand(const std::string &line, borisov::PersonNode **persons,
      borisov::DatedMeetingNode **meetings, RangeNode **rangeStack,
      const bool hasDate)
  {
    const borisov::Date dummyDate{0, 0, 0};
    const bool rangeEmpty = !hasDate || (*rangeStack)->empty_;
    const borisov::Date rangeStart = rangeEmpty ? dummyDate : (*rangeStack)->start_;
    const borisov::Date rangeEnd = rangeEmpty ? dummyDate : (*rangeStack)->end_;
    size_t pos = 0;
    std::string cmd;
    if (!parseWord(line, pos, cmd))
    {
      return false;
    }
    if (cmd == "anons")
    {
      return borisov::cmdAnons(std::cout, *persons, *meetings, rangeStart, rangeEnd);
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
      return borisov::cmdMeets(std::cout, *persons, *meetings, id, rangeStart, rangeEnd);
    }
    if (cmd == "commons")
    {
      size_t id1 = 0;
      size_t id2 = 0;
      if (!parseUnsigned(line, pos, id1) || !parseUnsigned(line, pos, id2))
      {
        return false;
      }
      return borisov::cmdCommons(std::cout, *persons, *meetings, id1, id2,
          rangeStart, rangeEnd);
    }
    if (cmd == "less")
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseUnsigned(line, pos, time) || !parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdLess(std::cout, *persons, *meetings, time, id,
          rangeStart, rangeEnd);
    }
    if (cmd == "greater")
    {
      size_t time = 0;
      size_t id = 0;
      if (!parseUnsigned(line, pos, time) || !parseUnsigned(line, pos, id))
      {
        return false;
      }
      return borisov::cmdGreater(std::cout, *persons, *meetings, time, id,
          rangeStart, rangeEnd);
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
    if (cmd == "range")
    {
      return cmdRange(std::cout, *rangeStack, hasDate);
    }
    if (cmd == "after")
    {
      size_t d = 0;
      size_t m = 0;
      size_t y = 0;
      if (!parseUnsigned(line, pos, d) || !parseUnsigned(line, pos, m) ||
          !parseUnsigned(line, pos, y))
      {
        return false;
      }
      return cmdAfter(rangeStack, *meetings, borisov::Date{d, m, y});
    }
    if (cmd == "before")
    {
      size_t d = 0;
      size_t m = 0;
      size_t y = 0;
      if (!parseUnsigned(line, pos, d) || !parseUnsigned(line, pos, m) ||
          !parseUnsigned(line, pos, y))
      {
        return false;
      }
      return cmdBefore(rangeStack, *meetings, borisov::Date{d, m, y});
    }
    if (cmd == "pop-range")
    {
      return cmdPopRange(rangeStack);
    }
    return false;
  }
}

int main(int argc, char **argv)
{
  borisov::Args args = borisov::parseArgs(argc, argv);
  if (!args.valid_)
  {
    std::cerr << "bad args\n";
    borisov::freeArgs(args);
    return 1;
  }
  borisov::PersonNode *persons = nullptr;
  if (args.inSet_)
  {
    std::ifstream inFile(args.inFile_.c_str());
    if (!inFile.is_open())
    {
      std::cerr << "cannot open file\n";
      borisov::freeArgs(args);
      return 2;
    }
    size_t count = 0;
    size_t skipped = 0;
    try
    {
      persons = borisov::readPersons(inFile, count, skipped);
    }
    catch (...)
    {
      std::cerr << "read error\n";
      borisov::freeArgs(args);
      return 2;
    }
  }
  borisov::DatedMeetingNode *meetings = nullptr;
  const borisov::StrNode *dataFile = args.dataFiles_;
  while (dataFile != nullptr)
  {
    std::ifstream df(dataFile->data_.c_str());
    if (!df.is_open())
    {
      borisov::freePersons(persons);
      borisov::freeDatedMeetings(meetings);
      borisov::freeArgs(args);
      std::cerr << "cannot open file\n";
      return 2;
    }
    borisov::DatedMeetingNode *part = nullptr;
    try
    {
      part = borisov::readDatedMeetings(df);
    }
    catch (...)
    {
      borisov::freePersons(persons);
      borisov::freeDatedMeetings(meetings);
      borisov::freeArgs(args);
      std::cerr << "invalid meeting data\n";
      return 3;
    }
    meetings = appendMeetings(meetings, part);
    dataFile = dataFile->next_;
  }
  borisov::freeArgs(args);
  borisov::Date minDate{0, 0, 0};
  borisov::Date maxDate{0, 0, 0};
  bool hasDate = false;
  computeRange(meetings, minDate, maxDate, hasDate);
  RangeNode *rangeStack = nullptr;
  if (hasDate)
  {
    rangeStack = new RangeNode{minDate, maxDate, false, nullptr};
  }
  std::string line;
  while (std::getline(std::cin, line))
  {
    if (!processCommand(line, &persons, &meetings, &rangeStack, hasDate))
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  freeRangeStack(rangeStack);
  borisov::freePersons(persons);
  borisov::freeDatedMeetings(meetings);
  return 0;
}
