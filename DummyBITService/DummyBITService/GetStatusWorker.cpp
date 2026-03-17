#include "GetStatusWorker.h"

GetStatusWorker::GetStatusWorker()
: m_publisher(nullptr)
{
    InitializePub();
}

GetStatusWorker::~GetStatusWorker() {

}

bool GetStatusWorker::InitializePub() {

    if (publisher.Initialize())
    {
        publisherCreated = 1;
        OutputDebugString(L"Publisher initialized\n");
    }
    else
    {
        OutputDebugString(L"Publisher initialization failed\n");
    }

    return 0;
}


bool GetStatusWorker::StatusRequested(double startTime)
{
    if (RunBIT(startTime)) {
        OutputDebugStringA("BIT successful\n");
    }
    else {
        OutputDebugStringA("BIT failed\n");
    }
    return TRUE;
}

bool GetStatusWorker::RunBIT(double startTime) {
    //insert code to simulate BIT
    SetStatus(1, startTime);
    return TRUE;
}

void GetStatusWorker::SetStatus(bool bitStatus, double startTime)
{
    OutputDebugStringA("\nStatus: Good\n");
    status = bitStatus;
    publisher.Publish(status, startTime);
}

bool GetStatusWorker::GetStatus()
{
    return status;
}