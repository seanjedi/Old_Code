//
// Tetromino.h
//

#ifndef TETROMINO_H
#define TETROMINO_H
class Tetromino
{
  public:
    virtual ~Tetromino(void) {}
    virtual char name(void) const = 0;
    virtual const char* color(void) const = 0;
    void print(void) const;
    int getX(int i) const;
    int getY(int i) const;
    bool overlap(const Tetromino &t) const;
    void draw(void) const;
    static Tetromino *makeTetromino(char ch,int posx,int posy,int orientation);
  protected:
    void rotate(int nrot);
    int x[4], y[4];
};

class I: public Tetromino
{
  public:
    I(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class O: public Tetromino
{
  public:
    O(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class T: public Tetromino
{
  public:
    T(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class J: public Tetromino
{
  public:
    J(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class L: public Tetromino
{
  public:
    L(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class S: public Tetromino
{
  public:
    S(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};

class Z: public Tetromino
{
  public:
    Z(int posx, int posy, int orientation);
    virtual char name(void) const;
    virtual const char* color(void) const; 
};
#endif
