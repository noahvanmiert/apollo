#
# Made by Noah Van Miery
# 27/01/2023
#

CC = clang
CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c11

SRC = src
EXEC = apollo
BIN = bin

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(SRCS:.c=.o)

$(BIN)/$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)/$(EXEC)

run:
	./$(BIN)/$(EXEC)

clean:
	rm -f $(BIN)/*
	rm -f $(SRC)/*.o
