CFLAGS=-O2 \
	-pedantic-errors \
	-Wall \
	-Werror \
	-std=c11 \
	-I "include"

SRCDIR=src
OBJDIR=obj

src2obj = $(subst .c,.o, \
	$(subst \
		$(SRCDIR), \
		$(OBJDIR), \
		$(1) \
	) \
)

SRC=$(wildcard src/tree.c src/hash.c src/filepath.c \
	src/lia/*.c src/lia/meta/*.c)
OBJ=$(call src2obj,$(SRC))

BIN=lia
INSTPATH=/usr/local/bin

all: starting $(OBJ) main.o
	$(CC) $(CFLAGS) $(OBJ) $(OBJDIR)/main.o -o $(BIN)

debug: starting $(OBJ) main.o
	$(CC) $(CFLAGS) -ggdb $(OBJ) $(OBJDIR)/main.o -o $(BIN)

starting:
	mkdir -p obj/lia/meta

main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/

doc:
	doxygen

install:
	mkdir -p ~/.lia
	cp $(BIN) $(INSTPATH)/
	cp -r modules ~/.lia/

uninstall:
	rm -r ~/.lia
	rm $(INSTPATH)/$(BIN)

test: starting $(OBJ)
	$(CC) $(CFLAGS) -DANSI_COLORS -c tests/test_$(name).c -o $(OBJDIR)/test_$(name).o
	$(CC) $(CFLAGS) $(OBJ) $(OBJDIR)/test_$(name).o -o test
	./test
	rm ./test