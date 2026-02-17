CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lconfuse -lpthread

TARGET = servidor

OBJ = main.o socket_utils.o config_parser.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lconfuse

main.o: main.c socket_utils.h config_parser.h
	$(CC) $(CFLAGS) -c main.c

socket_utils.o: socket_utils.c socket_utils.h
	$(CC) $(CFLAGS) -c socket_utils.c

config_parser.o: config_parser.c config_parser.h
	$(CC) $(CFLAGS) -c config_parser.c

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)