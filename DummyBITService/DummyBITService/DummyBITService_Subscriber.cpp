#include "DummyBITService_Subscriber.h"
#include "BitStreamConversion.h"

#include <cwchar>
#include <string>

// Constructor: initialize internal handles to null.
// Declare all workers needed for the service. This example only has the one worker
DummyBITService_Subscriber::DummyBITService_Subscriber()
    : m_subscriber(nullptr), worker_ptr(nullptr), m_appRuntimeStart(NULL)
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


// Initialize: initializes both the workers and subscriber listed in the constructor. Also, starts the "listening" of messages from other apps.
// Returns true on success, false on failure.
bool DummyBITService_Subscriber::Initialize()
{ 
    //Initialize worker
    worker_ptr = new GetStatusWorker();
    
    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background and post to UI
    try {
		// Setup subscriber to connect to proxy and list topics to subscribe to
		m_subscriber = std::make_unique<ZeroMQSubscriber>(PROXYBACKEND, std::vector<std::string>{
            
            // List of topics to subscribe to
            "StatusRequest"
        });

        // initialize subsbcriber and if successful, start loop to "listen" for subcribed topics
        if (m_subscriber->init()) {
            // start receiving; callback will post WM_ZMQ_MESSAGE to UI thread
            m_subscriber->start([this](const std::string& topic, std::unique_ptr<Message> message) {
                // if topic received, call worker to do work depending on topic.
                if (!topic.empty()) {
                    LPCSTR tmsg = topic.c_str();
                    OutputDebugStringA("\nReceived Topic: ");
                    OutputDebugStringA(tmsg);
                    //do work depending on topic
                    if (tmsg == "StatusRequest") {
                        m_appRuntimeStart = clock();
                        worker_ptr->StatusRequested(m_appRuntimeStart);
                        statReq = 1;
                    }
                }

                /*if (!message.empty()) {
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
                }*/
                } );

        }
        else {
            OutputDebugStringA("\nZeroMQ subscriber init failed\n");
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