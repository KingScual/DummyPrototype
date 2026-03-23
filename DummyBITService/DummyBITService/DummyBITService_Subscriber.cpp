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


// Initialize: initializes the subscribers and declares the workers listed in the constructor.
// Returns true on success, false on failure.
bool DummyBITService_Subscriber::Initialize()
{ 
    // Initializes worker_ptr;
    worker_ptr = new GetStatusWorker();
    
    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background.
    try {
		// Setup subscriber to connect to proxy and list topics to subscribe to
		m_subscriber = std::make_unique<ZeroMQSubscriber>(PROXYBACKEND, std::vector<std::string>{
            
            // List of topics to subscribe to
            "statusRequestFromDummyDataService"
        });

        // initialize subsbcriber and if successful, start loop to "listen" for subcribed topics
        if (m_subscriber->init()) {
            // start subscriber loop and listen for any published messages.
            m_subscriber->start([this](const std::string& topic, std::unique_ptr<Message> message) {
                // if topic received, then print to console and check the receiving topic.
                if (!topic.empty()) {
                    std::cout << "\nMessage Received:\n";
                    std::cout << "Topic: " << topic;
                    //do work depending on topic received
                    if (topic == "statusRequestFromDummyDataService") {
                        // start timer to keep track of how long a BIT request takes
                        m_appRuntimeStart = clock();
                        // call worker to start BIT request
                        worker_ptr->StatusRequested(m_appRuntimeStart);
                    }
                }
            } );

        }
        else {
            std::cout << "\nZeroMQ subscriber init failed\n";
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what();
    }

    return true;
}