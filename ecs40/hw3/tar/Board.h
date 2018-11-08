//
// Board.h
//
// Board for the Tetris game

#ifndef BOARD_H
#define BOARD_H
#include "Tetromino.h"
#include<vector>
class Board
{
  public:
    Board(int sx, int sy) : size_x(sx), size_y(sy) {}
    void addTetromino(char type, int x, int y, int orientation);
    bool fits_on_board(const Tetromino &t) const;
    void draw(void) const;
  private:
    Board(void);
    int size_x, size_y;
    std::vector<Tetromino *> tList;
};
#endif
