//
// testTetromino.cpp
//

#include "Tetromino.h"
#include <iostream>
#include <stdexcept>
using namespace std;

int main()
{
  Tetromino *t1, *t2;
  char ch;
  int x,y,orientation;

  try
  {
    cin >> ch >> x >> y >> orientation;
    t1 = Tetromino::makeTetromino(ch,x,y,orientation);
    t1->print();
    cin >> ch >> x >> y >> orientation;
    t2 = Tetromino::makeTetromino(ch,x,y,orientation);
    t2->print();
    if ( t1->overlap(*t2) )
      cout << "overlap" << endl;
  }
  catch ( invalid_argument &exc )
  {
    cout << exc.what() << ": " << ch << " " << x << " " << y << " "
         << orientation << endl;
  }

  return 0;
}
