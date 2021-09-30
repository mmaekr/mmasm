CC=gcc
INCLUDE_DIR=../include
SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o)
C_FLAGS:=-Wall -I$(INCLUDE_DIR)

all: $(OBJ)

%.o: %.c
	$(CC) -c $? $(C_FLAGS) -o $@

.PHONY: clean
clean:
	-@rm insns_* 2>/dev/null
