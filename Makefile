# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I. `sdl2-config --cflags`

# Linker flags
LDFLAGS = `sdl2-config --libs`

# Source files
SRCS = main.c graphics.c chip8.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = chip8_emulator

# Default rule
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)
