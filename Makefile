CC=gcc
CFLAGS=-fPIC -Wall -Werror -O2 -g
LDFLAGS=-shared
TARGET_LIB=libyata.so

INSTALL_LIB_DIR=/usr/local/lib
INSTALL_INC_DIR=/usr/local/include

YATA_SRC=yata.c
YATA_OBJ=yata.o

YATA_HDR=yata.h

all: $(TARGET_LIB)

$(TARGET_LIB): $(YATA_OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_LIB) $(YATA_OBJ)

clean:
	rm -rf $(YATA_OBJ) $(TARGET_LIB)

install: $(TARGET_LIB)
	cp $(TARGET_LIB) $(INSTALL_LIB_DIR)
	cp $(YATA_HDR) $(INSTALL_INC_DIR)
	ldconfig

uninstall:
	rm -rf $(INSTALL_LIB_DIR)/$(TARGET_LIB)
	rm -rf $(INSTALL_INC_DIR)/$(YATA_HDR)
	ldconfig

