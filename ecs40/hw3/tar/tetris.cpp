//
// tetris.cpp
//

#include "Tetromino.h"
#include "Board.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

int main()
{
  int size_x, size_y;
  // read the configuration from stdin
  cin >> size_x >> size_y;
  // create the board
  Board board(size_x,size_y);
  // add tetrominoes to the board as they are read
  char type;
  int x, y, orientation;

  try
  {
    cin >> type >> x >> y >> orientation;
    while ( cin )
    {
      board.addTetromino(type,x,y,orientation);
      cin >> type >> x >> y >> orientation;
    }
    // draw the board
    board.draw();
  }
  catch ( invalid_argument &e )
  {
    cout << e.what() << endl;
  }
}
