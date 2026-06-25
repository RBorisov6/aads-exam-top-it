#ifndef BORISOV_U3_COMMANDS_HPP
#define BORISOV_U3_COMMANDS_HPP
#include <cstddef>
#include <string>
#include <iosfwd>
#include "date.hpp"

namespace borisov
{
  struct PersonNode;
  struct DatedMeetingNode;

  bool cmdAnons(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, Date rangeStart, Date rangeEnd);
  bool cmdDeanon(DatedMeetingNode **meetings, const PersonNode *persons,
      size_t anonId, size_t id);
  bool cmdRedesc(PersonNode **head, const DatedMeetingNode *meetings,
      size_t id, const std::string &newInfo);
  bool cmdDesc(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, size_t id);
  bool cmdMeets(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, size_t id, Date rangeStart, Date rangeEnd);
  bool cmdCommons(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, size_t id1, size_t id2,
      Date rangeStart, Date rangeEnd);
  bool cmdLess(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, size_t time, size_t id,
      Date rangeStart, Date rangeEnd);
  bool cmdGreater(std::ostream &out, const PersonNode *persons,
      const DatedMeetingNode *meetings, size_t time, size_t id,
      Date rangeStart, Date rangeEnd);
  bool cmdOutPersons(const std::string &filename, const PersonNode *persons);
}

#endif
