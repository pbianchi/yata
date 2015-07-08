CC=gcc
CFLAGS=-fPIC -Wall -Werror -O2 -g -lpthread -lrt
LDFLAGS=-shared
TARGET_LIB=libyata.so

YATA_SRC=yata.c
YATA_OBJ=yata.o

all: $(TARGET_LIB)

$(TARGET_LIB): $(YATA_OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_LIB) $(YATA_OBJ)

clean:
	rm -rf $(YATA_OBJ) $(TARGET_LIB)

