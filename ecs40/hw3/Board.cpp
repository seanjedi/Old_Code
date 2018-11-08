//
//Sean Malloy
//998853013
//Tetromino.cpp
//

#include "Board.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

using namespace std;

void Board::addTetromino(char type, int x, int y, int orientation)
{
Tetromino *Halp = Halp->Tetromino::makeTetromino(type,x,y,orientation);
if(fits_on_board(*Halp))
{
  for(int i=0; abs(i) < tList.size(); i++)
    {
    if(tList[i]->overlap(*Halp))
	{
	throw invalid_argument("overlap");
	return;
	}
    }
tList.push_back(Halp);
}
else
{
	throw invalid_argument("does not fit on the board");
	return;
}
}

bool Board::fits_on_board(const Tetromino &t) const
{
for(int i=0; i<4 ; i++)
  {
	if( t.getX(i) < size_x && t.getY(i) < size_y)
	return true;
  }
return false;
}

void Board::draw(void) const
{
cout << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" <<endl;
cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" " << endl;
cout << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"> " << endl;
cout << "<svg width=\"570\" height=\"570\" viewBox=\"0 0 550 550\" " << endl;
cout << "xmlns=\"http://www.w3.org/2000/svg\" " << endl;
cout << "xmlns:xlink=\"http://www.w3.org/1999/xlink\" >" << endl;
cout << "<g transform=\"matrix(1,0,0,-1,50,550)\">" << endl;

cout << "<rect fill=\"white\" stroke=\"black\" x=\"0\" y=\"0\" width=\"" << size_x*50 << "\" height=\""<< size_y*50 << "\"/>" << endl;

for(int i =0; abs(i) < tList.size(); i++)
{
	tList[i]->draw();
}

cout << "</g>\n</svg>" << endl;
}
