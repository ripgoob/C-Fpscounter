# Compiler and flags
CC := x86_64-w64-mingw32-gcc
CFLAGS := -Wall -Wextra -std=c17 -IC:/msys64/mingw64/include/SDL2
LDFLAGS := -L C:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

# Output name
TARGET := fpscount.exe

# Source files
SRC := main.c

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean
clean:
	rm -f $(TARGET)
