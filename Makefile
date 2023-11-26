CC=gcc
CFLAGS=-g -Wall -Wextra
INCLUDES=-I/usr/include/SDL2/
LIBS=-L/usr/lib/x86_64-linux-gnu/cmake/SDL2 -lSDL2 -lSDL2_ttf

MATH=-lm

BIN_DIR=bin
TARGET=$(BIN_DIR)/main
SRC=*.c

all: $(TARGET)

main: $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LIBS) $(MATH) 

graph_visualizer: graph_visualizer.c
	$(CC) $(CLFAGS) $(INCLUDES) -o $(BIN_DIR)/$@ $< $(LIBS) $(MATH)

clean:
	rm -f $(BIN)
