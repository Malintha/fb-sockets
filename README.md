# fb-sockets
A simple UDP server using flatbuffers.

Install flatbuffers compiler as mentioned in https://stackoverflow.com/questions/55394537/how-to-install-flatc-and-flatbuffers-on-linux-ubuntu. Note that the
header files are already added to the include directory.

Do the following to build the example.

``make gen_headers`` 

``make build``

Execute ``./server`` to run the server and ``./client`` to run the client in two seperate terminals.
