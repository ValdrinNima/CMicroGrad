CC=gcc
CFLAGS=-g -Wall -Wextra

BIN=main
SRC=main.c

all: $(BIN)

main: $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN)
