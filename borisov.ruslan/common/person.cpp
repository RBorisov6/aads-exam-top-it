#include "person.hpp"
#include <cctype>
#include <istream>
#include <ostream>
#include <string>

bool borisov::containsId(const borisov::PersonNode * const head, const size_t id)
{
  const borisov::PersonNode *cur = head;
  while (cur != nullptr)
  {
    if (cur->data_.id_ == id)
    {
      return true;
    }
    cur = cur->next_;
  }
  return false;
}

borisov::PersonNode *borisov::readPersons(
  std::istream &in,
  size_t &count,
  size_t &skipped,
  bool &hasInput)
{
  borisov::PersonNode *head = nullptr;
  borisov::PersonNode *tail = nullptr;
  count = 0;
  skipped = 0;
  hasInput = false;
  try
  {
    std::string line;
    while (std::getline(in, line))
    {
      hasInput = true;
      size_t idStart = 0;
      while (idStart < line.size() && (line[idStart] == ' ' || line[idStart] == '\t'))
      {
        ++idStart;
      }
      if (idStart == line.size())
      {
        continue;
      }
      if (!std::isdigit(static_cast< unsigned char >(line[idStart])))
      {
        ++skipped;
        continue;
      }
      size_t idEnd = idStart;
      while (idEnd < line.size() && std::isdigit(static_cast< unsigned char >(line[idEnd])))
      {
        ++idEnd;
      }
      if (idEnd < line.size() && line[idEnd] != ' ' && line[idEnd] != '\t')
      {
        ++skipped;
        continue;
      }
      size_t id = 0;
      for (size_t i = idStart; i < idEnd; ++i)
      {
        id = id * 10 + static_cast< size_t >(line[i] - '0');
      }
      size_t infoStart = idEnd;
      while (infoStart < line.size() && (line[infoStart] == ' ' || line[infoStart] == '\t'))
      {
        ++infoStart;
      }
      if (infoStart >= line.size())
      {
        ++skipped;
        continue;
      }
      const std::string info = line.substr(infoStart);
      if (borisov::containsId(head, id))
      {
        ++skipped;
        continue;
      }
      borisov::PersonNode * const node = new borisov::PersonNode{borisov::Person{id, info}, nullptr};
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
      ++count;
    }
  }
  catch (...)
  {
    borisov::freePersons(head);
    throw;
  }
  return head;
}

void borisov::writePersons(std::ostream &out, const borisov::PersonNode * const head)
{
  const borisov::PersonNode *cur = head;
  while (cur != nullptr)
  {
    out << cur->data_.id_ << " " << cur->data_.info_ << "\n";
    cur = cur->next_;
  }
}

void borisov::freePersons(borisov::PersonNode * const head)
{
  borisov::PersonNode *cur = head;
  while (cur != nullptr)
  {
    borisov::PersonNode * const next = cur->next_;
    delete cur;
    cur = next;
  }
}
