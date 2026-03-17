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
        OutputDebugString(L"Publisher initialized\n");
    }
    else
    {
        OutputDebugString(L"Publisher initialization failed\n");
    }

     //Initialize Subscriber
    if (subscriber.Initialize())
    {
        subscriberCreated = 1;
        OutputDebugString(L"Subscriber initialized\n");
    }
    else
    {
        OutputDebugString(L"Subscriber initialization failed\n");
    }

    if (publisherCreated == 1 && subscriberCreated == 1) {
        return 1;
    }

    return 0;
}

bool RequestStatusWorker::RequestStatus(bool statusInit)
{
    if (publisherCreated)
    {
        bool statusReqPublished = publisher.Publish(statusInit);
    }
    else
    {
        OutputDebugString(L"Status Request could not be published\n");
    }
    return 0;
}

bool RequestStatusWorker::Status()
{
    bool state = subscriber.GetStatus();
    return state;
}