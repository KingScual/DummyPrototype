/*
* 
* DummyBITService_Initializer.cpp
* -------------------------------------------------------------------------------------------
* Description:
* - Contains the MAIN of the Dummy BIT Service
* - Initializes Subscriber classes needed for Dummy BIT Service (only 1)
* -------------------------------------------------------------------------------------------
* 
*/

#include "DummyBitService_Subscriber.h"

// Global Proxy Variables
const std::string PROXYFRONTEND = "tcp://localhost:5557";
const std::string PROXYBACKEND = "tcp://localhost:5558";

//Declare all needed subscribers
DummyBITService_Subscriber subscriber;

//Declare any other needed variables
bool previousStat = 0;
bool currentStat = 0;

int main()
{
	// Initialize Subscriber
     if (subscriber.Initialize())
    {
        std::cout << "Subscriber initialized\n";
    }
    else
    {
        std::cout << "Subscriber Failed to initialize\n";
    }

    // Infinite loop to allow app to continuously run until manual exit.
    while (true) {
       
    }
	return 0;
}