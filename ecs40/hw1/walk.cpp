//
// walk.cpp
//
#include "Walker.h"
#include<iostream>
using namespace std;

int main()
{
  Walker w;
  int dx=0, dy=0;
  try
  {
    cin >> w;
    cout << w << endl;
    cin >> dx >> dy;
    while ( cin )
    {
      w.move_by(dx,dy);
      cout << w << endl;
      cin >> dx >> dy;
    }
    cout << w << endl;
  }
  catch ( invalid_argument& e )
  {
    cout << "Error: " << e.what() << endl;
  }
}
