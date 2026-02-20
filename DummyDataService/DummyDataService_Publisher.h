#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>
#include "Proxy.h"
#include "ZeroMQ.h"
#include "zmq.hpp"

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

    // Initialize the Win32 window and controls. Returns true on success.
    bool Initialize();

private:

    // ZeroMQ publisher used to send messages when the button is clicked
    std::unique_ptr<ZeroMQPublisher> m_publisher;

};
