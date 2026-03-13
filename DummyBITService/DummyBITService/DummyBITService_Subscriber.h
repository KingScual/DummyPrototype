#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>


// Forward declare or include ZeroMQ publisher helper
#include "ZeroMQ.h"
// Include of Proxy port constants for Pubs/Subs connections
#include "Proxy.h"
#include "zmq.hpp"
#include "GetStatusWorker.h"

//std::string PROXYBACKEND = "tcp://localhost:5557";

class DummyBITService_Subscriber
{
public:
    // Construct the DummyDataService_Subscriber object and initialize member variables.
    DummyBITService_Subscriber();

    // Destroy the main window and unregister the window class.
    ~DummyBITService_Subscriber();

    // Initialize Subscriber. Return True on success.
    bool Initialize();

    bool GetStatReq();

private:

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
    GetStatusWorker* worker_ptr;
    bool statReq = 0;
};
