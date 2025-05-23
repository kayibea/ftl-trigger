CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC = src/main.c src/input.c src/vmouse.c
OBJ = $(SRC:.c=.o)
BIN = ftl-trigger

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(BIN) src/*.o
