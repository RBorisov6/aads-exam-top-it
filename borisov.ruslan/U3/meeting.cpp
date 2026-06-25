#include "meeting.hpp"
#include <istream>
#include <stdexcept>

borisov::DatedMeetingNode *borisov::readDatedMeetings(std::istream &in)
{
  borisov::DatedMeetingNode *head = nullptr;
  borisov::DatedMeetingNode *tail = nullptr;
  try
  {
    size_t day = 0;
    if (!(in >> day))
    {
      if (in.eof())
      {
        return nullptr;
      }
      throw std::runtime_error("invalid meeting data");
    }
    size_t month = 0;
    size_t year = 0;
    if (!(in >> month >> year))
    {
      throw std::runtime_error("invalid meeting data");
    }
    const borisov::Date date{day, month, year};
    size_t id1 = 0;
    while (in >> id1)
    {
      size_t id2 = 0;
      size_t duration = 0;
      if (!(in >> id2 >> duration))
      {
        throw std::runtime_error("invalid meeting data");
      }
      if (id1 == id2)
      {
        continue;
      }
      borisov::DatedMeetingNode * const node = new borisov::DatedMeetingNode{
          borisov::DatedMeeting{date, id1, id2, duration}, nullptr};
      if (tail == nullptr)
      {
        head = node;
        tail = node;
      }
      else
      {
        tail->next_ = node;
        tail = node;
      }
    }
    if (!in.eof())
    {
      throw std::runtime_error("invalid meeting data");
    }
  }
  catch (...)
  {
    borisov::freeDatedMeetings(head);
    throw;
  }
  return head;
}

void borisov::freeDatedMeetings(borisov::DatedMeetingNode * const head)
{
  borisov::DatedMeetingNode *cur = head;
  while (cur != nullptr)
  {
    borisov::DatedMeetingNode * const next = cur->next_;
    delete cur;
    cur = next;
  }
}
