//
//Person.cpp
//Name: Sean Malloy
//SID: 998853013
//Class: ECS40 Fall Quarter 2015
//



#include "Person.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>

using namespace std;

Person::Person(string last, string first, string num)
{
  for(unsigned int i = 0; i < last.length(); i++)
  {
    if(isalpha(last[i]) == 0)
    {
    cout << "invalid characters in last name" << endl;
    exit(1);
    }
  }

  for(unsigned int i = 0; i < first.length(); i++)
  {
    if(isalpha(first[i]) == 0)
    {
    cout << "invalid characters in first name" << endl;
    exit(1);
    }
  }
  
  for(int A = 0; A < 3; A++)
    {
    if(isdigit(num[A]) == 0)
      {
      cout << "invalid number format" << endl;
      exit(1);
      }
    }

  if(num[3]!='-')
      {
      cout << "invalid number format" << endl;
      exit(1);
      }

  for( int A = 4; A < 7; A++)
    {
    if(isdigit(num[A]) == 0)
    {
     cout << "invalid number format" << endl;
     exit(1);
    }
    }
    
    if(num[7]!='-')
      {
      cout << "invalid number format" << endl;
      exit(1);
      }
    
  for( int A = 8; A < 12; A++)
    {
    if(isdigit(num[A]) == 0)
    {
     cout << "invalid number format" << endl;
     exit(1);
    }
    }

lastname_ = last;
firstname_ = first;
number_ = num;
}

string Person::lastname(void) const
{
return lastname_;
}

string Person::firstname(void) const
{
return firstname_;
}

string Person::number(void) const
{
return number_;
}

bool Person::operator<(const Person &p) const
{
  if(lastname_ < p.lastname_)
    return 1;
  if(lastname_ > p.lastname_)
    return 0;
  if(lastname_ == p.lastname_)
  {
    if(firstname_ < p.firstname_)
    return 1;
    else if(firstname_ < p.firstname_)
    return 0;
  }
return 0;
}

ostream &operator<<(std::ostream& os, const Person &p)
{
os << setw(15) << left << p.lastname() << setw(15) << left << p.firstname() << p.number();
return os;
}
