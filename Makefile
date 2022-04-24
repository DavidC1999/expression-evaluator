CC=gcc
CFLAGS=-W -Wall -g -std=c11
LIBS=-lm
TARGET=calc
TARGET_PROD=$(TARGET)_prod

# globs
SRCS := $(wildcard source/*.c)
HDRS := $(wildcard source/*.h)
OBJS := $(patsubst source/%.c,bin/%.o,$(SRCS))

# link it all together
$(TARGET): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

# compile an object based on source and headers
bin/%.o: source/%.c $(HDRS) Makefile | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir $@

# tidy up
clean:
	rm -rf bin
	rm -f $(TARGET)
