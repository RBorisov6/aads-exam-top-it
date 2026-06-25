#include "commands.hpp"
#include <person.hpp>
#include <meeting.hpp>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

namespace
{
  struct IdDuration
  {
    size_t id_;
    size_t duration_;
  };

  void sortIdDuration(IdDuration * const arr, const size_t n)
  {
    for (size_t i = 1; i < n; ++i)
    {
      const IdDuration key = arr[i];
      size_t j = i;
      while (j > 0 && (arr[j - 1].id_ > key.id_ ||
          (arr[j - 1].id_ == key.id_ && arr[j - 1].duration_ > key.duration_)))
      {
        arr[j] = arr[j - 1];
        --j;
      }
      arr[j] = key;
    }
  }

  void sortIds(size_t * const arr, const size_t n)
  {
    for (size_t i = 1; i < n; ++i)
    {
      const size_t key = arr[i];
      size_t j = i;
      while (j > 0 && arr[j - 1] > key)
      {
        arr[j] = arr[j - 1];
        --j;
      }
      arr[j] = key;
    }
  }

  bool existsInMeetings(const borisov::MeetingNode * const meetings, const size_t id)
  {
    const borisov::MeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      if (cur->data_.id1_ == id || cur->data_.id2_ == id)
      {
        return true;
      }
      cur = cur->next_;
    }
    return false;
  }

  bool existsPerson(const borisov::PersonNode * const persons,
      const borisov::MeetingNode * const meetings, const size_t id)
  {
    return borisov::containsId(persons, id) || existsInMeetings(meetings, id);
  }

  size_t countMeetingsOf(const borisov::MeetingNode * const meetings, const size_t id)
  {
    size_t n = 0;
    const borisov::MeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      if (cur->data_.id1_ == id || cur->data_.id2_ == id)
      {
        ++n;
      }
      cur = cur->next_;
    }
    return n;
  }

  IdDuration *collectMeetingsOf(const borisov::MeetingNode * const meetings,
      const size_t id, size_t &count)
  {
    count = countMeetingsOf(meetings, id);
    if (count == 0)
    {
      return nullptr;
    }
    IdDuration * const arr = new IdDuration[count];
    size_t idx = 0;
    const borisov::MeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      if (cur->data_.id1_ == id)
      {
        arr[idx].id_ = cur->data_.id2_;
        arr[idx].duration_ = cur->data_.duration_;
        ++idx;
      }
      else if (cur->data_.id2_ == id)
      {
        arr[idx].id_ = cur->data_.id1_;
        arr[idx].duration_ = cur->data_.duration_;
        ++idx;
      }
      cur = cur->next_;
    }
    sortIdDuration(arr, count);
    return arr;
  }

  size_t *collectPartnersOf(const borisov::MeetingNode * const meetings,
      const size_t id, size_t &count)
  {
    const size_t total = countMeetingsOf(meetings, id);
    if (total == 0)
    {
      count = 0;
      return nullptr;
    }
    size_t * const tmp = new size_t[total];
    size_t tmpCount = 0;
    const borisov::MeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      size_t otherId = 0;
      bool found = false;
      if (cur->data_.id1_ == id)
      {
        otherId = cur->data_.id2_;
        found = true;
      }
      else if (cur->data_.id2_ == id)
      {
        otherId = cur->data_.id1_;
        found = true;
      }
      if (found)
      {
        bool dup = false;
        for (size_t i = 0; i < tmpCount; ++i)
        {
          if (tmp[i] == otherId)
          {
            dup = true;
            break;
          }
        }
        if (!dup)
        {
          tmp[tmpCount++] = otherId;
        }
      }
      cur = cur->next_;
    }
    sortIds(tmp, tmpCount);
    count = tmpCount;
    return tmp;
  }

  size_t *collectAnons(const borisov::PersonNode * const persons,
      const borisov::MeetingNode * const meetings, size_t &count)
  {
    size_t total = 0;
    const borisov::MeetingNode *cur = meetings;
    while (cur != nullptr)
    {
      total += 2;
      cur = cur->next_;
    }
    if (total == 0)
    {
      count = 0;
      return nullptr;
    }
    size_t * const tmp = new size_t[total];
    size_t tmpCount = 0;
    cur = meetings;
    while (cur != nullptr)
    {
      const size_t ids[2] = {cur->data_.id1_, cur->data_.id2_};
      for (size_t k = 0; k < 2; ++k)
      {
        const size_t id = ids[k];
        if (borisov::containsId(persons, id))
        {
          continue;
        }
        bool dup = false;
        for (size_t i = 0; i < tmpCount; ++i)
        {
          if (tmp[i] == id)
          {
            dup = true;
            break;
          }
        }
        if (!dup)
        {
          tmp[tmpCount++] = id;
        }
      }
      cur = cur->next_;
    }
    sortIds(tmp, tmpCount);
    count = tmpCount;
    return tmp;
  }
}

bool borisov::cmdAnons(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings)
{
  size_t count = 0;
  size_t * const anons = collectAnons(persons, meetings, count);
  if (count == 0)
  {
    out << "\n";
  }
  for (size_t i = 0; i < count; ++i)
  {
    out << anons[i] << "\n";
  }
  delete[] anons;
  return true;
}

bool borisov::cmdDesc(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings, const size_t id)
{
  if (!existsPerson(persons, meetings, id))
  {
    return false;
  }
  const borisov::PersonNode *cur = persons;
  while (cur != nullptr)
  {
    if (cur->data_.id_ == id)
    {
      out << cur->data_.info_ << "\n";
      return true;
    }
    cur = cur->next_;
  }
  out << "<ANON>\n";
  return true;
}

bool borisov::cmdMeets(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings, const size_t id)
{
  if (!existsPerson(persons, meetings, id))
  {
    return false;
  }
  size_t count = 0;
  IdDuration * const arr = collectMeetingsOf(meetings, id, count);
  if (count == 0)
  {
    out << "\n";
  }
  for (size_t i = 0; i < count; ++i)
  {
    out << arr[i].id_ << " " << arr[i].duration_ << "\n";
  }
  delete[] arr;
  return true;
}

bool borisov::cmdDeanon(borisov::MeetingNode **meetingsHead,
    const borisov::PersonNode * const persons, const size_t anonId, const size_t id)
{
  if (borisov::containsId(persons, anonId) || !borisov::containsId(persons, id))
  {
    return false;
  }
  if (!existsInMeetings(*meetingsHead, anonId))
  {
    return false;
  }
  borisov::MeetingNode *cur = *meetingsHead;
  while (cur != nullptr)
  {
    if (cur->data_.id1_ == anonId)
    {
      cur->data_.id1_ = id;
    }
    else if (cur->data_.id2_ == anonId)
    {
      cur->data_.id2_ = id;
    }
    cur = cur->next_;
  }
  borisov::MeetingNode *prev = nullptr;
  cur = *meetingsHead;
  while (cur != nullptr)
  {
    if (cur->data_.id1_ == cur->data_.id2_)
    {
      borisov::MeetingNode * const next = cur->next_;
      if (prev == nullptr)
      {
        *meetingsHead = next;
      }
      else
      {
        prev->next_ = next;
      }
      delete cur;
      cur = next;
    }
    else
    {
      prev = cur;
      cur = cur->next_;
    }
  }
  return true;
}

bool borisov::cmdRedesc(borisov::PersonNode **head,
    const borisov::MeetingNode * const meetings, const size_t id,
    const std::string &newInfo)
{
  if (!existsPerson(*head, meetings, id))
  {
    return false;
  }
  borisov::PersonNode *cur = *head;
  while (cur != nullptr)
  {
    if (cur->data_.id_ == id)
    {
      cur->data_.info_ = newInfo;
      return true;
    }
    cur = cur->next_;
  }
  borisov::PersonNode * const node = new borisov::PersonNode{borisov::Person{id, newInfo}, nullptr};
  borisov::PersonNode *cur2 = *head;
  if (cur2 == nullptr)
  {
    *head = node;
  }
  else
  {
    while (cur2->next_ != nullptr)
    {
      cur2 = cur2->next_;
    }
    cur2->next_ = node;
  }
  return true;
}

bool borisov::cmdCommons(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings, const size_t id1, const size_t id2)
{
  if (!existsPerson(persons, meetings, id1) || !existsPerson(persons, meetings, id2))
  {
    return false;
  }
  size_t count1 = 0;
  size_t * const partners1 = collectPartnersOf(meetings, id1, count1);
  size_t count2 = 0;
  size_t * const partners2 = collectPartnersOf(meetings, id2, count2);
  size_t i = 0;
  size_t j = 0;
  bool printed = false;
  while (i < count1 && j < count2)
  {
    if (partners1[i] == partners2[j])
    {
      out << partners1[i] << "\n";
      printed = true;
      ++i;
      ++j;
    }
    else if (partners1[i] < partners2[j])
    {
      ++i;
    }
    else
    {
      ++j;
    }
  }
  if (!printed)
  {
    out << "\n";
  }
  delete[] partners1;
  delete[] partners2;
  return true;
}

bool borisov::cmdLess(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings, const size_t time, const size_t id)
{
  if (!existsPerson(persons, meetings, id))
  {
    return false;
  }
  size_t count = 0;
  IdDuration * const arr = collectMeetingsOf(meetings, id, count);
  bool printed = false;
  for (size_t i = 0; i < count; ++i)
  {
    if (arr[i].duration_ < time)
    {
      out << arr[i].id_ << " " << arr[i].duration_ << "\n";
      printed = true;
    }
  }
  if (!printed)
  {
    out << "\n";
  }
  delete[] arr;
  return true;
}

bool borisov::cmdGreater(std::ostream &out, const borisov::PersonNode * const persons,
    const borisov::MeetingNode * const meetings, const size_t time, const size_t id)
{
  if (!existsPerson(persons, meetings, id))
  {
    return false;
  }
  size_t count = 0;
  IdDuration * const arr = collectMeetingsOf(meetings, id, count);
  bool printed = false;
  for (size_t i = 0; i < count; ++i)
  {
    if (arr[i].duration_ > time)
    {
      out << arr[i].id_ << " " << arr[i].duration_ << "\n";
      printed = true;
    }
  }
  if (!printed)
  {
    out << "\n";
  }
  delete[] arr;
  return true;
}

bool borisov::cmdOutPersons(const std::string &filename,
    const borisov::PersonNode * const persons)
{
  std::ofstream out(filename.c_str(), std::ios::app);
  if (!out.is_open())
  {
    return false;
  }
  if (persons == nullptr)
  {
    out << "\n";
    std::cout << "\n";
    return true;
  }
  borisov::writePersons(out, persons);
  borisov::writePersons(std::cout, persons);
  return true;
}
