//
//Sean Malloy
//998853013
//Tetromino.cpp
//

#include "Tetromino.h"
#include <iostream>
#include <stdexcept>

using namespace std;

Tetromino *Tetromino::makeTetromino(char ch, int posx, int posy, int orientation)
{
	if(orientation >= 0 && orientation <= 3)
	{
		if (ch == 'I')
			return new I(posx, posy, orientation);
		else if (ch == 'O')
			return new O(posx, posy, orientation);
		else if (ch == 'T')
			return new T(posx, posy, orientation);
		else if (ch == 'J')
			return new J(posx, posy, orientation);
		else if (ch == 'L')
			return new L(posx, posy, orientation);
		else if (ch == 'S')
			return new S(posx, posy, orientation);
		else if (ch == 'Z')
			return new Z(posx, posy, orientation);
		else throw invalid_argument ("invalid type");
	}
	else
		throw invalid_argument ("invalid orientation");
}


char I::name(void) const
{
	return 'I';
}


I::I(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx;
	x[2] = posx;
	x[3] = posx;

	y[0] = posy;
	y[1] = posy+1;
	y[2]= posy+2;
	y[3] = posy+3;

	rotate(orientation);
}


char O::name(void) const
{
	return 'O';
}

O::O(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx+1;
	x[2] = posx;
	x[3] = posx+1;

	y[0] = posy;
	y[1] = posy;
	y[2]= posy+1;
	y[3] = posy+1;

	rotate(orientation);
}


char T::name(void) const
{
	return 'T';
}

T::T(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx-1;
	x[2] = posx;
	x[3] = posx+1;

	y[0] = posy;
	y[1] = posy+1;
	y[2]= posy+1;
	y[3] = posy+1;

	rotate(orientation);
}


char J::name(void) const
{
	return 'J';
}

J::J(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx+1;
	x[2] = posx+1;
	x[3] = posx+1;

	y[0] = posy;
	y[1] = posy;
	y[2]= posy+1;
	y[3] = posy+2;

	rotate(orientation);
}


char L::name(void) const
{
	return 'L';
}

L::L(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx+1;
	x[2] = posx;
	x[3] = posx;

	y[0] = posy;
	y[1] = posy;
	y[2]= posy+1;
	y[3] = posy+2;

	rotate(orientation);
}


char S::name(void) const
{
	return 'S';
}

S::S(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx+1;
	x[2] = posx+1;
	x[3] = posx+2;

	y[0] = posy;
	y[1] = posy;
	y[2]= posy+1;
	y[3] = posy+1;

	rotate(orientation);
}


char Z::name(void) const
{
	return 'Z';
}

Z::Z(int posx, int posy, int orientation) 
{
	x[0] = posx;
	x[1] = posx+1;
	x[2] = posx-1;
	x[3] = posx;

	y[0] = posy;
	y[1] = posy;
	y[2]= posy+1;
	y[3] = posy+1;

	rotate(orientation);
}

bool Tetromino::overlap(const Tetromino &t) const
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{ 
			if(t.x[i] == x[j] && t.y[i] == y[j])
				return true;
		}
	}
	return false;
}


void Tetromino::print(void) const
{
	cout<< name() << " at (" << x[0] << "," << y[0] 
		<< ") (" << x[1] << ","  << y[1] 
		<< ") (" << x[2] << ","  << y[2] 
		<< ") (" << x[3] << ","  << y[3] 
		<< ")" << endl;
}

void Tetromino::rotate(int nrot)
{
	int tempx,tempy;

	for(int i = 0; i<nrot; i++)
	{
		for(int j = 1; j<4; j++)
		{
			tempx = x[0] + y[0] - y[j];
			tempy = y[0] - x[0] + x[j];
			x[j] = tempx;
			y[j] = tempy;
		}
	}
}

