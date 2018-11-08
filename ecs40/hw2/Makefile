all: testTetromino

CXXFLAGS=-g -Wall

Tetromino.o: Tetromino.cpp Tetromino.h
testTetromino.o: testTetromino.cpp Tetromino.h

testTetromino: testTetromino.o Tetromino.o
	$(CXX) -o $@ $^  $(LDFLAGS)

clean:
	rm -f *.o testTetromino
