#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>
#include <sstream>
//#include "Proxy.h"
#include "ZeroMQ.h"
#include "zmq.hpp"

//std::string PROXYFRONTEND = "tcp://localhost:5556";

// Forward-declare ZeroMQPublisher to avoid pulling in the full ZeroMQ header here.
// The destructor and implementation live in the .cpp which includes the complete type.
class ZeroMQPublisher;

// Simple application class that wraps a Win32 window and a button.
class DummyDataService_Publisher
{ 
public:
    // Construct the DummyDataService_Publisher object and initialize member variables.
    DummyDataService_Publisher();

    // Destroy the main window and unregister the window class.
    ~DummyDataService_Publisher();

    //Initialize Publisher. Returns true on success.
    bool Initialize();

    bool Publish(bool msg);

private:

    // ZeroMQ publisher used to send messages when the button is clicked
    std::unique_ptr<ZeroMQPublisher> m_publisher;

};
