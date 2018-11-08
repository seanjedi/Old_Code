all: walk useWalker

walk: walk.cpp Walker.cpp Walker.h
	g++ -Wall -g -c walk.cpp Walker.cpp
	g++ -Wall -g -o walk walk.o Walker.o

useWalker: useWalker.cpp Walker.cpp Walker.h
	g++ -Wall -g -c useWalker.cpp Walker.cpp
	g++ -Wall -g -o useWalker Walker.o useWalker.o 

clean: 
	rm -f useWalker walk
