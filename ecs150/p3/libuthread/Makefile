FLAGS := -Wall -Werror
CC-FLAGS1 := gcc $(CFLAGS)
lib := libuthread.a  # Target library

all: $(lib)

$(lib): queue.o sem.o thread.o tps.o
	ar rcs $(lib) queue.o sem.o thread.o tps.o
	ranlib $(lib)

# thread.o not declared because it cannot be rebuilt during compilation
# queue.o not declared because it cannot be rebuilt during compilation

tps.o: tps.c tps.h
	$(CC-FLAGS1) -c -o tps.o tps.c

sem.o: sem.c sem.h
	$(CC-FLAGS1) -c -o sem.o sem.c

clean:
	rm -f $(lib) sem.o tps.o
## TODO: Phase 1 and Phase 2

