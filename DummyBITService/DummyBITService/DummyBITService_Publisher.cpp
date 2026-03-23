#include "DummyBITService_Publisher.h"

#include <cwchar>
#include <string>

// Constructor: initialize internal handles to null.
DummyBITService_Publisher::DummyBITService_Publisher()
    : m_publisher(nullptr)
{
}

// Destructor
DummyBITService_Publisher::~DummyBITService_Publisher()
{
    // m_publisher will be cleaned up automatically (its destructor calls close())
}

// Initialize: Initialize publisher
// Returns true on success, false on failure.
bool DummyBITService_Publisher::Initialize()
{
    // Initialize ZeroMQ publisher to connect to the proxy frontend socket
    try {
       m_publisher = std::make_unique<ZeroMQPublisher>(PROXYFRONTEND);
        if (!m_publisher->init()) {
            // Initialization failed
            std::cout << "ZeroMQ publisher init failed\n";
        }
    }
    catch (const std::exception& ex) {
        // If ZeroMQ or allocation throws, log but continue running the UI
        std::cout << ex.what();
    }
    return true;
}

// Publishes ZMQ messages found in Messages.h
bool DummyBITService_Publisher::Publish(bool msg, double startTime)
{
    // Declare message
    AppStatus message;

    // Fill message depending on system status
    if (msg)
    {
        message.appId = "BIT Service";
        message.appHealth = "Good";
        message.appRuntime = (clock() - startTime) / CLOCKS_PER_SEC;
    }
    else {
        message.appId = "BIT Service";
        message.appHealth = "Bad";
        message.appRuntime = (clock() - startTime) / CLOCKS_PER_SEC;
    }

    // Publish message
    bool published = m_publisher->publish("statusResponseToDummyDataService",message);
    
    if (published)
    {
        std::cout << "Status publihsed successfully\n";
    }
    else
    {
        std::cout << "Status could not publish\n";
    }

    return true;
}