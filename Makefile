CFLAGS = -I./include -Wall -g

USE_READLINE ?= 1
LDLIBS =

ifeq ($(USE_READLINE),1)
CFLAGS += -DUSE_READLINE=1
LDLIBS += -lreadline
endif

TARGET = thinsh

SRCS = src/main.c src/vars.c src/builtins.c src/sig.c src/jobs.c \
	   src/compiler.c src/interpreter.c src/loadopcode.c src/trang_builtin.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run

clean:
	rm -f src/*.o $(TARGET)
