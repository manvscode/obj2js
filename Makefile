#CFLAGS = -std=c11 -O0 -Wall -fsanitize=undefined -g -I /usr/local/include -I extern/include/
CFLAGS = -std=c11 -O2 -Wall -fsanitize=undefined -I /usr/local/include -I extern/include/
LDFLAGS = extern/lib/libutility.a extern/lib/libcollections.a -L /usr/local/lib -L extern/lib/ -L extern/libcollections/lib/
CWD = $(shell pwd)
BIN_NAME = obj2js

SOURCES = src/main.c src/libobj.c

all: extern/libutility extern/libcollections bin/$(BIN_NAME)

bin/$(BIN_NAME): $(SOURCES:.c=.o)
	@mkdir -p bin
	@echo "CFLAGS=$(CFLAGS)"
	@echo "Linking: $^"
	@$(CC) $(CFLAGS) -o bin/$(BIN_NAME) $^ $(LDFLAGS)
	@echo "Created $@"

src/%.o: src/%.c
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

#################################################
# Dependencies                                  #
#################################################
extern/libutility:
	@mkdir -p extern/libutility/
	@git clone https://bitbucket.org/manvscode/libutility.git extern/libutility/
	@cd extern/libutility && autoreconf -i && ./configure --libdir=$(CWD)/extern/lib/ --includedir=$(CWD)/extern/include/ && make && make install

extern/libcollections:
	@mkdir -p extern/libcollections/
	@git clone https://bitbucket.org/manvscode/libcollections.git extern/libcollections/
	@cd extern/libcollections && autoreconf -i && ./configure --libdir=$(CWD)/extern/lib/ --includedir=$(CWD)/extern/include/ && make && make install

#################################################
# Cleaning                                      #
#################################################
clean_extern:
	@rm -rf extern

clean:
	@rm -rf src/*.o
	@rm -rf bin
