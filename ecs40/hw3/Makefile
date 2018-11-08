all: tetris

CXXFLAGS=-g -Wall

Tetromino.o: Tetromino.cpp Tetromino.h
Board.o: Board.cpp Board.h Tetromino.h
tetris.o: tetris.cpp Tetromino.h Board.h

tetris: Tetromino.o Board.o tetris.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o tetris
