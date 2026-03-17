#include "DummyBITService_Publisher.h"

#include <cwchar>
#include <string>

// Constructor: initialize internal handles to null.
DummyBITService_Publisher::DummyBITService_Publisher()
    : m_publisher(nullptr)
{
}

// Destructor: destroy the main window if created and unregister the window class.
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
            OutputDebugStringA("ZeroMQ publisher init failed\n");
        }
    }
    catch (const std::exception& ex) {
        // If ZeroMQ or allocation throws, log but continue running the UI
        OutputDebugStringA(ex.what());
    }
    return true;
}

bool DummyBITService_Publisher::Publish(bool msg, double startTime)
{
    AppStatus message;
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
    bool published = m_publisher->publish("Status",message);
    if (published)
    {
        OutputDebugString(L"Status publihsed successfully\n");
    }
    else
    {
        OutputDebugString(L"Status could not publish\n");
    }
    return true;
}