CC=emcc
CFLAGS=-I./ -O2 -std=c99
LIBS=../src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 -DPLATFORM_WEB

SRC=main.c
OUT=index.html
RAYLIB_PATH=..

all:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LIBS) -I$(RAYLIB_PATH)/src -L$(RAYLIB_PATH)/src
