//
//changeOfBase.cpp
//Sean Malloy 998853013
//1-20-2016
//

#include <iostream>
#include <string>

using namespace std;
int main()
{
int power, base1, base2, num1, anum1, num2;
double  remainder;

cout << "Please enter the number's base: " << endl;
cin >> base1;
cout << "Please enter the number: " << endl;
cin >> num1;

anum1=num1;
power = 0;

while (anum1>0)
{
power++;
num1 = anum1/10;
}

sum = 0;
anum = num1;
while(anum > 0)
{
sum += (anum%10)*(pow(base1,power));
anum = num/10;
power--;
}

cout << "Please enter the new base: " << endl;
cin >> base2;


//
string baseConv(int num1,int base1, int base2)
{
string result;
int numA = base2;
  for(int numA = abs(num1); numA > 0; numA /= base1)
  {
    int temp  =  numA % base1;
    if((temp >=0) && (temp <= 9))
      result += ('0' + temp);
    else
      result += ('A' + (temp - 10));
  }
std::reverse(result.begin(), result.end());
return result;
}
//

cout << num1 << "base" << base1 << "is" << num2 << "base" << baseConv(num1,base1, base2) << endl;

return 0;
}
