#
# this makefile will compile and and all source
# for the Server application for CWT System
#
#==============================================

# FINAL BINARY Target
./bin/Chat-Server : ./obj/server.o
	cc -g ./obj/server.o -o ./bin/cwtserver
#
# =======================================================
#                     Dependencies
# =======================================================

./obj/server.o : ./src/server.c ./inc/chatServer.h
	cc -g -c ./src/server.c -o ./obj/server.o


#
# =======================================================
# Other targets
# =======================================================
all : ./bin/server

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~
