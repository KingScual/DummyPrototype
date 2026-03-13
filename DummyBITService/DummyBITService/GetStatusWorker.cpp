#include "GetStatusWorker.h"

GetStatusWorker::GetStatusWorker() {
    //InitializePub();
}

GetStatusWorker::~GetStatusWorker() {

}

/*bool GetStatusWorker::InitializePub() {

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
*/

bool GetStatusWorker::StatusRequested()
{
    if (RunBIT()) {
        OutputDebugStringA("BIT successful\n");
    }
    else {
        OutputDebugStringA("BIT failed\n");
    }
    return TRUE;
}

bool GetStatusWorker::RunBIT() {
    //insert code to simulate BIT
    SetStatus(1);
    return TRUE;
}

void GetStatusWorker::SetStatus(bool bitStatus)
{
    OutputDebugStringA("Status: Good\n");
    status = bitStatus;
}

bool GetStatusWorker::GetStatus()
{
    return status;
}