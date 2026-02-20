#include "DummyDataService_Subscriber.h"
#include "C:\DummyServicePrototype\DummyPrototype\BitStreamConversion\BitStreamConversion.h"

#include <cwchar>
#include <string>
#include "zmq.hpp"
/*
// Constructor: initialize internal handles to null.
DummyDataService_Subscriber::DummyDataService_Subscriber()
    : m_hReceiveEdit(nullptr), m_subscriber(nullptr)
{
}

// Destructor: destroy the main window if created and unregister the window class.
DummyDataService_Subscriber::~DummyDataService_Subscriber()
{
    // Stop and destroy the subscriber if present
    if (m_subscriber)
    {
        try { m_subscriber->stop(); m_subscriber->close(); }
        catch (...) {}
        m_subscriber.reset();
    }
}


// Initialize: register the window class, create the main window and child controls (button).
// Returns true on success, false on failure.
bool DummyDataService_Subscriber::Initialize()
{
    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background and post to UI
    try {
		// Connect to the same multicast group; subscribe to topic 'Status' to receive status updates from the service
		m_subscriber = std::make_unique<ZeroMQSubscriber>(PROXYBACKEND, std::vector<std::string>{"Status"}); // subscribe to Status topic
        if (m_subscriber->init()) {
            // start receiving; callback will post WM_ZMQ_MESSAGE to UI thread
            m_subscriber->start([this](const std::string& topic, const bool& message) {
                // save the status message
				bool status = message;
                if (status != NULL) {
					//Code to display the status message in DummyDataService UI
                }
                });
        }
        else {
            OutputDebugStringA("ZeroMQ subscriber init failed\n");
        }
    }
    catch (const std::exception& ex) {
        OutputDebugStringA(ex.what());
    }

    return true;
}*/