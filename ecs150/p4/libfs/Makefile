FLAGS := -Wall -Werror
CC-FLAGS1 := gcc $(CFLAGS)
lib := libfs.a   # Target library

all: $(lib)

$(lib): disk.o fs.o
	ar rcs $(lib) disk.o fs.o
	ranlib $(lib)

disk.o: disk.c disk.h
	$(CC-FLAGS1) -c -o disk.o disk.c

fs.o: fs.c fs.h
	$(CC-FLAGS1) -c -o fs.o fs.c

clean:
	rm -f $(lib) disk.o fs.o
