#
# this makefile will compile and and all source
# found in the chat-client directory.
#
#==============================================

# FINAL BINARY Target
./bin/chat-client : ./obj/chat-client.o ./obj/ui.o
	cc ./obj/chat-client.o ./obj/ui.o -lncurses -o  ./bin/chat-client
#
# =======================================================
#                     Dependencies
# =======================================================

./obj/chat-client.o : ./src/chat-client.c  ./inc/chat-client.h
	cc -c ./src/chat-client.c -o  ./obj/chat-client.o

./obj/ui.o : ./src/ui.c  ./inc/chat-client.h
	cc -c ./src/ui.c -o  ./obj/ui.o


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
