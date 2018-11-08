//
//scientificFloating.cpp
//Sean Malloy
//1-20-2016
//998853013
//

#include <iostream>
#include <string>
using namespace std;

int main()
{

union a{
float num;
int integer;
};

a thing;
string result;
cout << "Please enter a float: " << endl;
cin >> thing.num;

for(int i=31; i>=0; i--)
{
 if((thing.integer & (1<<i)))
   cout << 1;
 else
   cout << 0;
}

for(int i=0; i<=31; i++)
{
 if((thing.integer & (1<<i)))
   cout << 1;
 else
   cout << 0;
}

 if((thing.integer & (1<<31)))
   cout << "-";
}
