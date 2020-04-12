#
# this makefile will call all the makefiles required
# for the chat-server and chat-client applications.
#
#==============================================

./bin:
	cd chat-client && make && cp ./bin/chat-client ../Common/bin
	cd chat-server && make && cp ./bin/chat-server ../Common/bin

#
# =======================================================
# Other targets
# =======================================================
all :  ./bin/CWT/

clean:
	rm -f ./Common/bin/*
	rm -f ./chat-client/bin/*
	rm -f ./chat-client/obj/*.o
	rm -f ./chat-client/inc/*.h~
	rm -f ./chat-client/src/*.c~
	rm -f ./chat-server/bin/*
	rm -f ./chat-server/obj/*.o
	rm -f ./chat-server/inc/*.h~
	rm -f ./chat-server/src/*.c~
	rm -f ./CWTServer.log
