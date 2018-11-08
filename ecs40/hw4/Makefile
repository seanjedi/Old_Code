XXFLAGS=-Wall
CXX=g++
all: phonebook
Person.o: Person.cpp Person.h
phonebook.o: phonebook.cpp Person.h 
phonebook: phonebook.o Person.o 
	$(CXX) -o $@ $^
clean:
	rm -f *.o phonebook
