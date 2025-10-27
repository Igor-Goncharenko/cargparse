CC = gcc
CFLAGS = -Wall -Wextra -std=c89 -O3 -I..
LDFLAGS = ../libcargparse.a

TARGET = cargparse_tests

OBJ = test_core.o tests.o

.PHONY: clean lib

all: clean lib $(TARGET)

lib:
	@cd .. && $(MAKE) clean && $(MAKE)

$(TARGET): $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJ) $(TARGET)
