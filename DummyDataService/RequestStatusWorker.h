#pragma once

#include "DummyDataService_Publisher.h"
#include "DummyDataService_Subscriber.h"

class RequestStatusWorker
{
public:
    RequestStatusWorker();
    ~RequestStatusWorker();
    bool InitializePubSub();
    bool RequestStatus(bool statusInit); //return status

private:
	bool publisherCreated = 0;
    bool subscriberCreated = 0;

    DummyDataService_Publisher publisher; //declare publisher for dummy data service
    DummyDataService_Subscriber subscriber; //declare subscriber for dummy data service
};