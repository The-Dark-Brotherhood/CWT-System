#
# this makefile will compile and and all source
# found in the chat-client directory.
#
#==============================================

# FINAL BINARY Target
./bin/chat-client : ./obj/chat-client.o ./obj/ui.o ./obj/client-socket.o
	cc ./obj/chat-client.o ./obj/ui.o ./obj/client-socket.o -lncurses -o  ./bin/chat-client -lpthread
#
# =======================================================
#                     Dependencies
# =======================================================

./obj/chat-client.o : ./src/chat-client.c  ./inc/chat-client.h
	cc -c ./src/chat-client.c -o  ./obj/chat-client.o

./obj/ui.o : ./src/ui.c  ./inc/chat-client.h
	cc -c ./src/ui.c -o  ./obj/ui.o

./obj/client-socket.o : ./src/client-socket.c ./inc/chat-client.h
	cc -c ./src/client-socket.c -o ./obj/client-socket.o


#
# =======================================================
# Other targets
# =======================================================
all : ./bin/chat-client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~
