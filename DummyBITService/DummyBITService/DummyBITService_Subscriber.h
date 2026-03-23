/*
* 
* DummyBITService_Subscriber.h
* --------------------------------------------------------------
* Description:
* - Contains initialization of subscriber class
* - Initializes all worker classes (only 1)
* - Begins subscriber loop to "listen" for subscribed topics
* --------------------------------------------------------------
* 
*/

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "ZeroMQ.h"
#include "Proxy.h"
#include "zmq.hpp"
#include "GetStatusWorker.h"
#include "Messages.h"

class DummyBITService_Subscriber
{
public:
    // Constructor - Construct the DummyDataService_Subscriber object and initialize member variables.
    DummyBITService_Subscriber();

    // Destructor - Destroys subcriber if they exist
    ~DummyBITService_Subscriber();

    // Initialize Subscriber and declare worker classes. Return True on success.
    bool Initialize();

private:

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
    // a double used to keep track of runtime when BIT status requested.
    double m_appRuntimeStart;
    // GetStatusWorker pointer used to point to worker class.
    GetStatusWorker* worker_ptr;
};
