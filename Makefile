CFLAGS ?= -Wall -Wextra
LDFLAGS ?=
CC ?= gcc
STATIC ?= 0
DEBUG ?= 0

BIN = bst_test
SRC = bst.c main.c
OBJ = $(SRC:.c=.o)
INCLUDE= -I ./include/

#Mandatory flags
CFLAGS += ${INCLUDE}

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g -fno-inline
else
	CFLAGS += -DNDEBUG -O3
endif

ifeq ($(STATIC), 1)
	LDFLAGS += -static
endif

all: $(BIN)

$(BIN) : $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS) $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean :
	@rm -f $(OBJ) $(BIN)
