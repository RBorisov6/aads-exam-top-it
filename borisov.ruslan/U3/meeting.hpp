#ifndef BORISOV_U3_MEETING_HPP
#define BORISOV_U3_MEETING_HPP
#include <cstddef>
#include <iosfwd>
#include "date.hpp"

namespace borisov
{
  struct DatedMeeting
  {
    Date date_;
    size_t id1_;
    size_t id2_;
    size_t duration_;
  };

  struct DatedMeetingNode
  {
    DatedMeeting data_;
    DatedMeetingNode *next_;
  };

  DatedMeetingNode *readDatedMeetings(std::istream &in);
  void freeDatedMeetings(DatedMeetingNode *head);
}

#endif
