#
# Made by Noah Van Miery
# 27/01/2023
#

CC = clang
CFLAGS =  -Wall -Wextra -Werror -pedantic -std=c11

SRC = src
EXEC = apollo
BIN = bin

SRCS = $(wildcard $(SRC)/*.c $(SRC)/**/*.c)
OBJS = $(SRCS:.c=.o)

$(BIN)/$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)/$(EXEC)

run:
	./$(BIN)/$(EXEC)

make-output:
	as -arch arm64 -o output.o output.s
	ld -o output output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64

run-output:
	./output

clean:
	rm -f $(BIN)/*
	rm -f $(SRC)/*.o
	rm -f $(SRC)/**/*.o
