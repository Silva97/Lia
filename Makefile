CFLAGS=-O2         \
	-pedantic-errors  \
	-Wall            \
	-Werror           \
	-std=c11         \
	-I "include"

SRC=src/tree.c src/hash.c src/filepath.c src/lia/*.c src/lia/meta/*.c
BIN=lia
INSTPATH=/usr/local/bin

all:
	$(CC) $(CFLAGS) -DNDEBUG $(SRC) src/main.c -o $(BIN)

debug:
	$(CC) $(CFLAGS) -ggdb $(SRC) src/main.c -o $(BIN)

doc:
	doxygen

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