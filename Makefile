CC=gcc
CFLAGS=-W -Wall -g -std=c11
TARGET=calc
TARGET_PROD=$(TARGET)_prod

# globs
SRCS := $(wildcard source/*.c)
HDRS := $(wildcard source/*.h)
OBJS := $(patsubst source/%.c,bin/%.o,$(SRCS))

# link it all together
$(TARGET): $(OBJS) $(HDRS) makefile
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# compile an object based on source and headers
bin/%.o: source/%.c $(HDRS) makefile
	$(CC) $(CFLAGS) -c $< -o $@

# tidy up
clean:
	del /Q bin
	del $(TARGET).exe