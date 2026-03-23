#include <random>
#include "GetStatusWorker.h"

// Constructor
GetStatusWorker::GetStatusWorker()
{
    //Initialize publisher
    std::cout << "Worker Initialized\n";
    if (publisher.Initialize())
    {
        publisherCreated = 1;
        std::cout << "Publisher initialized\n";
    }
    else
    {
        std::cout << "Publisher initialization failed\n";
    }
}

// Desctructor
GetStatusWorker::~GetStatusWorker() {

}

// IBIT requested
bool GetStatusWorker::StatusRequested(double startTime)
{
    bool r_flag = false;
    // Trigger simulated BIT
    if (RunBIT(startTime)) {
        std::cout << "BIT successful\n";
        r_flag = true;
    }
    else {
        std::cout << "BIT failed\n";
    }
    return r_flag;
}

// Simulated BIT API calls and publish results
bool GetStatusWorker::RunBIT(double startTime) {
    
    // simulate BIT by generating random number. If even, then status good; if odd, then status bad
   // srand(time(0));
   // int randomNum = rand() % 101;
    //std::cout << "RANDOMNUM: " << randomNum;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,100);
    int randomNum = dist6(rng);
    bool randomStatus = 0;
    if (randomNum % 2 == 0) {
        randomStatus = 1;
    }
    
    // Set status member variable with BIT result
    SetStatus(randomStatus);

    // Publish BIT result
    if (publisherCreated) {
        publisher.Publish(status, startTime);
    }
    else {
        std::cout << "Could not publish. Publisher socket not created.\n";
    }
    return true;
}

// Sets status member variable
void GetStatusWorker::SetStatus(bool bitStatus)
{
    std::string stringStatus = (bitStatus) ? "Good" : "Bad";
    std::cout << "\nStatus: " << stringStatus << "\n";
    status = bitStatus;
}

// Returns status member variable
bool GetStatusWorker::GetStatus()
{
    return status;
}