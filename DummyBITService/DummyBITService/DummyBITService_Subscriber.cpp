#include "DummyBITService_Subscriber.h"
#include "BitStreamConversion.h"

#include <cwchar>
#include <string>

// Constructor: initialize internal handles to null.
DummyBITService_Subscriber::DummyBITService_Subscriber()
    : m_subscriber(nullptr)
{
}

// Destructor
DummyBITService_Subscriber::~DummyBITService_Subscriber()
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
bool DummyBITService_Subscriber::Initialize()
{
    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background and post to UI
    try {
		// Connect to the same multicast group; subscribe to topic 'StatusInit' to receive status updates from the service
		m_subscriber = std::make_unique<ZeroMQSubscriber>("tcp://localhost:5556", std::vector<std::string>{""}); // subscribe to StatusInit topic
        if (m_subscriber->init()) {
            // start receiving; callback will post WM_ZMQ_MESSAGE to UI thread
            m_subscriber->start( [this](const std::string& topic, const std::string& message) {
                // save the status message
                //bool status = message;
                //if (!message.empty()) {
                    //LPCSTR msg = message.c_str();
                    //OutputDebugStringA(msg);
                    //Code to display the status message in DummyDataService UI
                //}
                } );
        }
        else {
            OutputDebugStringA("ZeroMQ subscriber init failed\n");
        }
    }
    catch (const std::exception& ex) {
        OutputDebugStringA(ex.what());
    }

    return true;
}