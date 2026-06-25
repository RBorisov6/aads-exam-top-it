#ifndef BORISOV_PERSON_HPP
#define BORISOV_PERSON_HPP
#include <cstddef>
#include <string>
#include <iosfwd>

namespace borisov
{
  struct Person
  {
    size_t id_;
    std::string info_;
  };

  struct PersonNode
  {
    Person data_;
    PersonNode *next_;
  };

  bool containsId(const PersonNode *head, size_t id);
  PersonNode *readPersons(std::istream &in, size_t &count, size_t &skipped, bool &hasInput);
  void writePersons(std::ostream &out, const PersonNode *head);
  void freePersons(PersonNode *head);
}

#endif
