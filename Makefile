# To build for Linux, FreeBSD, etc..:
# 	make OS=unix
#
# To build for Windows x86:
# 	make OS=windows-x86
#
# To build for Windows x86_64:
# 	make OS=windows-x86_64

ifndef $(OS)
OS=unix
endif

ifndef $(DEBUG)
DEBUG=false
endif

CWD = $(shell pwd)

CFLAGS = -std=c11 -Wall \
		 -I/usr/local/include \
		 -Iextern/include/collections-1.0.0/ \
		 -Iextern/include/xtd-1.0.0/ \
		 -Iextern/include/obj-1.0.0/
LDFLAGS = 

ifeq ($(DEBUG), true)
	CFLAGS += -O2
else
	CFLAGS += -O0 -g
endif


ifeq ($(OS),unix)
	BIN_NAME = obj2js
	CC = gcc
	HOST=
	CFLAGS += -fsanitize=undefined
	LDFLAGS += -L/usr/local/lib \
			   -L$(CWD)/extern/lib/ \
			   -lobj \
			   -lxtd \
			   -lcollections
endif

ifeq ($(OS),windows-x86)
	BIN_NAME = obj2js-x86.exe
	CC=i686-w64-mingw32-gcc
	HOST=i686-w64-mingw32
	CFLAGS += -D_POSIX -mconsole
	LDFLAGS += -Wl,-no-undefined -L$(CWD)/extern/lib/ -L/usr/i686-w64-mingw32/lib/ \
			   -lobj \
			   -lxtd \
			   -lcollections \
			   \
			   -lmingw32 \
			   -lgcc \
			   -lgcc_eh \
			   -lmoldname \
			   -lmingwex \
			   -lmsvcrt \
			   -ladvapi32 \
			   -lshell32 \
			   -luser32 \
			   -lkernel32
endif

ifeq ($(OS),windows-x86_64)
	BIN_NAME = obj2js-x64.exe
	CC=x86_64-w64-mingw32-gcc
	HOST=x86_64-w64-mingw32
	CFLAGS += -D_POSIX -mconsole
	LDFLAGS += -Wl,-no-undefined -L$(CWD)/extern/lib/ -L/usr/x86_64-w64-mingw32/lib/ \
			   -lobj \
			   -lxtd \
			   -lcollections \
			   \
			   -lmingw32 \
			   -lgcc \
			   -lgcc_eh \
			   -lmoldname \
			   -lmingwex \
			   -lmsvcrt \
			   -ladvapi32 \
			   -lshell32 \
			   -luser32 \
			   -lkernel32
endif

SOURCES = src/main.c

#################################################
# Build Rules                                   #
#################################################
all: extern/libxtd \
	 extern/libcollections \
	 extern/libobj \
	 bin/$(BIN_NAME)

bin/$(BIN_NAME): $(SOURCES:.c=.o)
	@mkdir -p bin
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
	@cd extern/libxtd && \
		autoreconf -fi && \
		./configure --libdir=$(CWD)/extern/lib/ \
		            --includedir=$(CWD)/extern/include/ \
					--disable-shared \
					--host=$(HOST) && \
		make && \
		make install

extern/libcollections:
	@mkdir -p extern/libcollections/
	@git clone https://github.com/manvscode/libcollections.git extern/libcollections/
	@cd extern/libcollections && \
		autoreconf -fi && \
		./configure --libdir=$(CWD)/extern/lib/ \
		            --includedir=$(CWD)/extern/include/ \
					--disable-shared \
					--host=$(HOST) && \
		make && \
		make install

extern/libobj: extern/libcollections extern/libxtd
	@mkdir -p extern/libobj/
	@git clone https://github.com/manvscode/libobj.git extern/libobj/
	@cd extern/libobj && \
		autoreconf -i && \
		PKG_CONFIG_LIBDIR=$(CWD)/extern/lib/pkgconfig \
		./configure --libdir=$(CWD)/extern/lib/ \
		            --includedir=$(CWD)/extern/include/ \
					--disable-shared \
					--host=$(HOST) && \
		make && \
		make install

#################################################
# Cleaning                                      #
#################################################
clean_extern:
	@rm -rf extern

clean:
	@rm -rf src/*.o
	@rm -rf bin

#################################################
# Installing                                    #
# #################################################
install:
ifeq ("$(INSTALL_PATH)","")
	$(error INSTALL_PATH is not set.)
endif
	@echo "Installing ${CWD}/bin/${BIN_NAME} to ${INSTALL_PATH}"
	@cp bin/$(BIN_NAME) $(INSTALL_PATH)/$(BIN_NAME)
