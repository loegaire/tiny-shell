# -g adds debug info (so you can use gdb/valgrind)
CFLAGS = -I./include -Wall -g

# The target executable name
TARGET = thinsh

# Source files
# Update this line when you add new files (e.g., src/feature_vars.c)
SRCS = src/main.c

# Object files (automatically converts .c to .o in the list)
OBJS = $(SRCS:.c=.o)

# Default rule: build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- NEW: Run Command ---
# Usage: make run
run: $(TARGET)
	./$(TARGET)

# .PHONY tells Make that 'clean' and 'run' are not real files
.PHONY: clean run

# Clean up build files
clean:
	rm -f src/*.o $(TARGET)
