CC=gcc
CFLAGS=-fPIC -Wall -Werror -O2 -g
LDFLAGS=-shared
TARGET_LIB=libyata.so

INSTALL_DIR=/usr/local/lib

YATA_SRC=yata.c
YATA_OBJ=yata.o

all: $(TARGET_LIB) install

$(TARGET_LIB): $(YATA_OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_LIB) $(YATA_OBJ)

clean:
	rm -rf $(YATA_OBJ) $(TARGET_LIB)

install: $(TARGET_LIB)
	cp $(TARGET_LIB) $(INSTALL_DIR)
	ldconfig

uninstall:
	rm -rf $(INSTALL_DIR)/$(TARGET_LIB)

