CC=gcc
CFLAGS += -I/usr/include/commons
LIBS += -lcommons

# Archivos fuente
SRCS=test_memoria.c pagination.c
OBJS=$(SRCS:.c=.o)
TARGET=test_memoria

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) test_memoria.log