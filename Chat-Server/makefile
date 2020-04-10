#
# this makefile will compile and and all source
# for the Server application for CWT System
#
#==============================================

# FINAL BINARY Target
./bin/Chat-Server : ./obj/server.o ./obj/clientList.o ./obj/communications.o ./obj/logger.o
	cc -g ./obj/server.o ./obj/clientList.o ./obj/communications.o ./obj/logger.o -o ./bin/Chat-Server -lpthread
#
# =======================================================
#                     Dependencies
# =======================================================

./obj/server.o : ./src/server.c ./inc/chatServer.h
	cc -g -c ./src/server.c -o ./obj/server.o

./obj/communications.o : ./src/communications.c ./inc/chatServer.h
		cc -c ./src/communications.c -o ./obj/communications.o

./obj/clientList.o : ./src/clientList.c ./inc/chatServer.h
		cc -c ./src/clientList.c -o ./obj/clientList.o

./obj/logger.o : ./src/logger.c ./inc/logger.h
		cc -c ./src/logger.c -o ./obj/logger.o

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/Chat-Server

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~
