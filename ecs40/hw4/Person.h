//
// Person.h
//

#ifndef PERSON_H 
#define PERSON_H
#include<iostream>
#include<string>

class Person
{
  public:
    Person(std::string last, std::string first, std::string num);
    std::string lastname(void) const;
    std::string firstname(void) const;
    std::string number(void) const;
    bool operator<(const Person& p) const;
  private:
    std::string lastname_;
    std::string firstname_;
    std::string number_;
};
std::ostream& operator<< (std::ostream& os, const Person& p);
#endif
