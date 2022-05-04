#CC = clang
CWD = $(shell pwd)
CFLAGS = -std=c11 -O2 -Wall -fsanitize=undefined \
		 -I /usr/local/include \
		 -I$(CWD)/extern/include/collections-1.0.0/ \
		 -I$(CWD)/extern/include/xtd-1.0.0/ \
		 -I$(CWD)/extern/include/obj-1.0.0/
LDFLAGS = -L/usr/local/lib -L$(CWD)/extern/lib/ \
          -l:libobj.a \
          -l:libxtd.a \
          -l:libcollections.a
BIN_NAME = obj2js

SOURCES = src/main.c

all: extern/libxtd \
	 extern/libcollections \
	 extern/libobj \
	 bin/$(BIN_NAME)

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
extern/libxtd:
	@mkdir -p extern/libxtd/
	@git clone https://github.com/manvscode/libxtd.git extern/libxtd/
	@cd extern/libxtd && autoreconf -i && ./configure --libdir=$(CWD)/extern/lib/ --includedir=$(CWD)/extern/include/ && make && make install

extern/libcollections:
	@mkdir -p extern/libcollections/
	@git clone https://github.com/manvscode/libcollections.git extern/libcollections/
	@cd extern/libcollections && autoreconf -i && ./configure --libdir=$(CWD)/extern/lib/ --includedir=$(CWD)/extern/include/ && make && make install

extern/libobj: extern/libcollections extern/libxtd
	@mkdir -p extern/libobj/
	@git clone https://github.com/manvscode/libobj.git extern/libobj/
	@cd extern/libobj && autoreconf -i && ./configure --libdir=$(CWD)/extern/lib/ --includedir=$(CWD)/extern/include/ && make && make install

#################################################
# Cleaning                                      #
#################################################
clean_extern:
	@rm -rf extern

clean:
	@rm -rf src/*.o
	@rm -rf bin
