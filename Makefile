CC=gcc
CFLAGS=-g -Wall -Wextra
MATH=-lm

BIN=main
SRC=main.c

all: $(BIN)

main: $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(MATH) 

clean:
	rm -f $(BIN)
