// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <zmq.hpp>



int main()
{
	//make context thread
	void* context = zmq_ctx_new();

	//make frontend SUB and backend PUB socket to take (sub) and re-direct (pub) messages
	void* frontend = zmq_socket(context, ZMQ_XSUB);
	void* backend = zmq_socket(context, ZMQ_XPUB);

	//bind to ports which will be hard coded to Dummy1, 2, 3
	zmq_bind(frontend, "tcp://*:5557");
	zmq_bind(backend, "tcp://*:5558");

	//start the proxy, runs until context is closed
	std::cout << "Proxy Opened" << std::endl;	
	zmq_proxy(frontend, backend, NULL);
	
	return 0;


}


