gen_headers:
	flatc --cpp messages/states.fbs
	mv states_generated.h messages/

build:
	g++ client.cpp -I./include -o client
	g++ server.cpp -I./include -o server

clean:
	rm client server