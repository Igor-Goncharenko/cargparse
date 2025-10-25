CC = gcc
CFLAGS = -Wall -Wextra -fPIC -std=c89 -O3
LDFLAGS =
AR = ar
ARFLAGS = rcs

LIBNAME = libcargparse
LIB = $(LIBNAME).a

OBJ = cargparse.o

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ) 
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(LIB)

