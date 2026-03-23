#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Forward declare or include ZeroMQ publisher helper
#include "ZeroMQ.h"
// Include of Proxy port constants for Pubs/Subs connections
#include "Proxy.h"
#include "zmq.hpp"
#include "Messages.h"

//std::string PROXYBACKEND = "tcp://localhost:5557";

class DummyDataService_Subscriber
{
public:
    // Construct the DummyDataService_Subscriber object and initialize member variables.
    DummyDataService_Subscriber();

    // Destroy the main window and unregister the window class.
    ~DummyDataService_Subscriber();

    // Initialize Subscriber. Return True on success.
    bool Initialize();
    bool GetStatus();

private:

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
    std::string status;
};