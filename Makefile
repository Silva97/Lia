CC=gcc
CFLAGS=-std=c11 -I "include"
SRC=src/lia/*.c

all:
	$(CC) $(CFLAGS) $(SRC) src/main.c -o lia

test:
	$(CC) $(CFLAGS) -DANSI_COLORS $(SRC) tests/test_$(name).c -o test
	./test
	rm ./test