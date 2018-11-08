//
//phonebook.cpp
//Name: Sean Malloy
//SID: 998853013
//Class: ECS40 Fall Quarter 2015
//


#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "Person.h"
#include <set>

using namespace std;

int main(int argc, char* argv[1])
{
set <Person> P;

string lastN, firstN, phoneN;

ifstream tester;
tester.open(argv[1]);
if(!tester)
{
  cout << "file not found" << endl;
  exit(EXIT_SUCCESS);
}

while(tester>>lastN>>firstN>>phoneN)
{
  Person per(lastN, firstN, phoneN);
  P.insert(per);
}

set<Person>::iterator iter;

for(iter = P.begin(); iter!=P.end(); iter++)
{
cout << *iter << endl;
}
}

