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

// Initialize: register the window class, create the main window and child controls (button).
// Returns true on success, false on failure.
bool DummyBITService_Publisher::Initialize()
{
    // Initialize ZeroMQ publisher to connect to the proxy frontend socket
    // Initialize ZeroMQ publisher to send messages when the button is clicked.
    try {
       m_publisher = std::make_unique<ZeroMQPublisher>(PROXYFRONTEND);
        if (!m_publisher->init()) {
            // Initialization failed; keep the pointer so publish() can attempt init lazily.
            OutputDebugStringA("ZeroMQ publisher init failed\n");
        }
    }
    catch (const std::exception& ex) {
        // If ZeroMQ or allocation throws, log but continue running the UI
        OutputDebugStringA(ex.what());
    }
    return true;
}

bool DummyBITService_Publisher::Publish(bool msg)
{
    std::string msg_str = msg ? "true" : "false";
    bool published = m_publisher->publish("Status",msg_str);
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