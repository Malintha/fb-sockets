gen_headers:
	flatc --cpp messages/states.fbs
	mv states_generated.h messages/

	flatc --cpp messages/neighborhoods.fbs
	mv neighborhoods_generated.h messages/

build:
	g++ client.cpp -I./include -o client
	g++ server.cpp -I./include -o server

clean:
	rm -r messages/*.h
	rm client server