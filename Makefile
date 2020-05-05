CC=gcc
CFLAGS=-O2 -std=c11 -I "include"
SRC=src/tree.c src/hash.c src/filepath.c src/lia/*.c
BIN=lia
INSTPATH=/usr/local/bin

all:
	$(CC) $(CFLAGS) $(SRC) src/main.c -o $(BIN)

install:
	mkdir -p ~/.lia
	cp $(BIN) $(INSTPATH)/
	cp -r modules ~/.lia/

uninstall:
	rm -r ~/.lia
	rm $(INSTPATH)/$(BIN)

test:
	$(CC) $(CFLAGS) -DANSI_COLORS $(SRC) tests/test_$(name).c -o test
	./test
	rm ./test