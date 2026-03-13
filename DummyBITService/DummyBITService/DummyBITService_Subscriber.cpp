#include "DummyBITService_Subscriber.h"
#include "BitStreamConversion.h"

#include <cwchar>
#include <string>

// Constructor: initialize internal handles to null.
DummyBITService_Subscriber::DummyBITService_Subscriber()
    : m_subscriber(nullptr), worker_ptr(nullptr)
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
    worker_ptr = new GetStatusWorker();
    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background and post to UI
    try {
        //declare worker classes;
		// Connect to the same multicast group; subscribe to topic 'StatusInit' to receive status updates from the service
		m_subscriber = std::make_unique<ZeroMQSubscriber>(PROXYBACKEND, std::vector<std::string>{"StatusInit"}); // subscribe to StatusInit topic
        if (m_subscriber->init()) {
            // start receiving; callback will post WM_ZMQ_MESSAGE to UI thread
            m_subscriber->start([this](const std::string& topic, const std::string& message) {
                // save the status message
                if (!topic.empty()) {
                    LPCSTR tmsg = topic.c_str();
                    OutputDebugStringA("\nReceived Topic: ");
                    OutputDebugStringA(tmsg);
                }
                if (!message.empty()) {
                    LPCSTR msg = message.c_str();
                    OutputDebugStringA("\nReceived Message: ");
                    OutputDebugStringA(msg);
                    if (message == "true")
                    {
                        if (worker_ptr->StatusRequested()) {
                            OutputDebugStringA("Status successfully requested\n");
                            statReq = 1;
                        }
                        else {
                            OutputDebugStringA("Status could not be requested\n");
                        }
                    }
                }
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

bool DummyBITService_Subscriber::GetStatReq()
{
    return statReq;
}