#ifndef BORISOV_U2_COMMANDS_HPP
#define BORISOV_U2_COMMANDS_HPP
#include <cstddef>
#include <string>
#include <iosfwd>

namespace borisov
{
  struct PersonNode;
  struct MeetingNode;

  bool cmdAnons(std::ostream &out, const PersonNode *persons, const MeetingNode *meetings);
  bool cmdDeanon(MeetingNode **meetings, const PersonNode *persons, size_t anonId, size_t id);
  bool cmdRedesc(PersonNode **head, const MeetingNode *meetings, size_t id,
      const std::string &newInfo);
  bool cmdDesc(std::ostream &out, const PersonNode *persons,
      const MeetingNode *meetings, size_t id);
  bool cmdMeets(std::ostream &out, const PersonNode *persons,
      const MeetingNode *meetings, size_t id);
  bool cmdCommons(std::ostream &out, const PersonNode *persons,
      const MeetingNode *meetings, size_t id1, size_t id2);
  bool cmdLess(std::ostream &out, const PersonNode *persons,
      const MeetingNode *meetings, size_t time, size_t id);
  bool cmdGreater(std::ostream &out, const PersonNode *persons,
      const MeetingNode *meetings, size_t time, size_t id);
  bool cmdOutPersons(const std::string &filename, const PersonNode *persons);
}

#endif
