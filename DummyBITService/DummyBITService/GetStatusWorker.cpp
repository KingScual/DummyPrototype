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
    OutputDebugStringA("\nStatus: Good\n");
    status = bitStatus;
    publisher.Publish(status);
}

bool GetStatusWorker::GetStatus()
{
    return status;
}