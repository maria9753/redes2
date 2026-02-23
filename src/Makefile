CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lconfuse -lpthread

TARGET = server

OBJ = main.o socket_utils.o config_parser.o http_parser.o picohttpparser.o response_handler.o script_handler.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

main.o: main.c socket_utils.h config_parser.h http_parser.h response_handler.h
	$(CC) $(CFLAGS) -c main.c

socket_utils.o: socket_utils.c socket_utils.h
	$(CC) $(CFLAGS) -c socket_utils.c

config_parser.o: config_parser.c config_parser.h
	$(CC) $(CFLAGS) -c config_parser.c

http_parser.o: http_parser.c http_parser.h picohttpparser.h
	$(CC) $(CFLAGS) -c http_parser.c

picohttpparser.o: picohttpparser.c picohttpparser.h
	$(CC) $(CFLAGS) -c picohttpparser.c

response_handler.o: response_handler.c response_handler.h script_handler.h
	$(CC) $(CFLAGS) -c response_handler.c

script_handler.o: script_handler.c script_handler.h
	$(CC) $(CFLAGS) -c script_handler.c

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)