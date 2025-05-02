CC = emcc
SRC = main.c
OUT = index.html
RAYLIB_PATH = ..
LIBS = ../src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 -DPLATFORM_WEB
CFLAGS = -I$(RAYLIB_PATH)/src -O2 -std=c99
LDFLAGS = $(LIBS) -L$(RAYLIB_PATH)/src/web

all:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)
