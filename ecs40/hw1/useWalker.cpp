//
// useWalker.cpp
//

#include "Walker.h"
#include<iostream>
using namespace std;

int main()
{

  try
  {
    Walker w(2,3);

    cout << w << endl;

    cin >> w;
    cout << w << endl;

    w.move_by(1,0);
    w.move_by(0,1);
    w.move_by(-2,0);
    w.move_by(0,-2);
    w.move_by(2,0);
    w.move_by(0,1);
    w.move_by(-1,0);

    cout << w << endl;

    Walker ww(w);
    cout << ww.get_x() << " " << ww.get_y() << endl;

    Walker www;
    www = w;
    cout << www << endl;

  }
  catch ( invalid_argument& e )
  {
    cout << "Error: " << e.what() << endl;
  }
}
