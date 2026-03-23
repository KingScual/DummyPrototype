#include "RequestStatusWorker.h"

RequestStatusWorker::RequestStatusWorker() {
    InitializePubSub();
}

RequestStatusWorker::~RequestStatusWorker() {

}

bool RequestStatusWorker::InitializePubSub(){

    if (publisher.Initialize())
    {
        publisherCreated = 1;
        std::cout << "Publisher initialized\n";
    }
    else
    {
        std::cout << "Publisher initialization failed\n";
    }

     //Initialize Subscriber
    if (subscriber.Initialize())
    {
        subscriberCreated = 1;
        std::cout << "Subscriber initialized\n";
    }
    else
    {
        std::cout << "Subscriber initialization failed\n";
    }

    if (publisherCreated == 1 && subscriberCreated == 1) {
        return 1;
    }

    return 0;
}

bool RequestStatusWorker::RequestStatus(int num)
{
    if (publisherCreated)
    {
        for (int i = 0; i < num; i++) {
            std::cout << i+1 << ": ";
            publisher.Publish();
        }
    }
    else
    {
        std::cout << "Status Request could not be published\n";
    }
    return 0;
}

bool RequestStatusWorker::Status()
{
    bool state = subscriber.GetStatus();
    return state;
}