CC=gcc
CFLAGS=-g -Wall -Wextra
INCLUDES=-I/usr/include/SDL2/
LIBS=-L/usr/lib/x86_64-linux-gnu/cmake/SDL2 -lSDL2 -lSDL2_ttf ./Roboto-Medium.ttf

MATH=-lm

BIN=main
SRC=main.c

all: $(BIN)

main: $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN) $(SRC) $(LIBS) $(MATH) 

graph_visualizer: graph_visualizer.c
	$(CC) $(CLFAGS) $(INCLUDES) -o $@ $< $(LIBS)

clean:
	rm -f $(BIN)
