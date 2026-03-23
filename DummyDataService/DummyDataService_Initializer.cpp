#include <chrono>
#include <thread>
#include "RequestStatusWorker.h"

const std::string PROXYFRONTEND = "tcp://localhost:5557";
const std::string PROXYBACKEND = "tcp://localhost:5558";

RequestStatusWorker requester;

bool statusInit = 0;
bool status;

int main()
{
	int request, num;
	bool exitService = 0;

	//pause for 2 seconds to allow workers/pubs/subs to initialize
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::cout << "\nChoose one of the following commands and press enter (only enter cooresponding integer value):\n"
			<< "1. BIT request\n"
		    << "0. Exit\n\n";

	while (!exitService) {
		std::cout << "\nEnter here: ";
		std::cin >> request;
		switch (request) {
		case 0:
			exitService = 1;
			break;
		case 1:
			std::cout << "How many BITs do you want to run?: ";
			std::cin >> num;
			requester.RequestStatus(num);
			break;
		default:
			std::cout << request << " is not an option\n\n";
		}

		//pause for 1 second to allow calls to complete
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}