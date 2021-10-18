

CC=g++
CFLAGS=-pthread

default: server client

server: Server.cpp Server.h Parser.h
	$(CC) $(CFLAGS) -o server Server.cpp Parser.h

client: Client.cpp Client.h Parser.h
	$(CC) $(CFLAGS) -o client Client.cpp Parser.h
