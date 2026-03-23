/*
* 
* DummyBITService_Publisher.h
* --------------------------------------------------------------
* Description:
* - Publishes ZMQ messages.
* --------------------------------------------------------------
* 
*/
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <sstream>
#include "Proxy.h"
#include "ZeroMQ.h"
#include "zmq.hpp"
#include "Messages.h"

// Forward-declare ZeroMQPublisher to avoid pulling in the full ZeroMQ header here.
// The destructor and implementation live in the .cpp which includes the complete type.
class ZeroMQPublisher;

class DummyBITService_Publisher
{ 
public:
    // Construct the DummyBITService_Publisher object and initialize member variables.
    DummyBITService_Publisher();

    // Destroy the main window and unregister the window class.
    ~DummyBITService_Publisher();

    //Initialize Publisher. Returns true on success.
    bool Initialize();

    // Publishes ZMQ messages
    bool Publish(bool msg, double startTime);

private:

    // ZeroMQ publisher used to send messages.
    std::unique_ptr<ZeroMQPublisher> m_publisher;

};