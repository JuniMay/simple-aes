
CC = clang

BIN_DIR = ./build
SRC_DIR = ./src

.PHONY: lut init clean

lut: $(SRC_DIR)/lut.c init
	CC -o $(BIN_DIR)/lut $(SRC_DIR)/lut.c
	$(BIN_DIR)/lut

aes: $(SRC_DIR)/aes.c init
	CC -o $(BIN_DIR)/aes $(SRC_DIR)/aes.c
	$(BIN_DIR)/aes

init:
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)