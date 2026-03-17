#pragma once

#include "DummyBitService_Publisher.h"
#include <Windows.h>
#include <memory>

class GetStatusWorker
{
public:
    GetStatusWorker();
    ~GetStatusWorker();
    bool InitializePub();
    bool StatusRequested();
    void SetStatus(bool bitStatus);
    bool GetStatus(); //return status

private:
	bool publisherCreated = 0;
    bool subscriberCreated = 0;
    bool status = 0; // 0 == bad; 1 == good
    DummyBITService_Publisher publisher;
    std::unique_ptr<ZeroMQPublisher> m_publisher;

    bool RunBIT();
};