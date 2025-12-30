// ZeroMQ publisher implementation
// Provides a simple thread-safe publisher wrapper using cppzmq (zmq.hpp).

#include "ZeroMQ.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cerrno>

// Constructor
// - store the bind address, create a ZMQ context with one IO thread
// - socket is not created until init() is called
ZeroMQPublisher::ZeroMQPublisher(const std::string& bindAddress)
    : bindAddress_(bindAddress),
    context_(1),
    socket_(nullptr),
    initialized_(false)
{
}

// Destructor
// - ensure resources are cleaned up by calling close()
ZeroMQPublisher::~ZeroMQPublisher()
{
    close();
}

// init()
// - Creates and binds a ZMQ PUB socket to the configured address
// - Sets socket options (linger = 0 so close returns quickly)
// - Small sleep to allow subscribers to connect (optional)
// - Thread-safe using mutex
// - Returns true on successful initialization
bool ZeroMQPublisher::init()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_)
        return true;

    try {
        socket_ = std::make_unique<zmq::socket_t>(context_, zmq::socket_type::pub);
        // Set linger to 0 so close returns quickly
        int linger = 0;
        socket_->set(zmq::sockopt::linger, linger);

        // Bind the publisher socket to the configured address (e.g. "tcp://*:5556")
        socket_->bind(bindAddress_);

        // Give subscribers a moment to connect (optional small pause)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        initialized_ = true;
        return true;
    }
    catch (const zmq::error_t& e) {
        // Report init errors and reset state
        std::cerr << "ZeroMQPublisher init error: " << e.what() << "\n";
        socket_.reset();
        initialized_ = false;
        return false;
    }
}

// publish(topic, message)
// - Ensures the socket is initialized, then sends a multipart message:
//   first frame = topic, second frame = message
// - Uses a mutex to make publishing thread-safe
// - Any ZMQ errors are caught and logged
bool ZeroMQPublisher::publish(const std::string& topic, const std::string& message)
{
    // Ensure socket is initialized
    if (!initialized_) {
        if (!init())
            return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    try {
        // Send topic as first frame
        zmq::const_buffer topicBuf(topic.data(), topic.size());
        socket_->send(topicBuf, zmq::send_flags::sndmore);

        // Send message as second frame
        zmq::const_buffer msgBuf(message.data(), message.size());
        socket_->send(msgBuf, zmq::send_flags::none);

        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQPublisher publish error: " << e.what() << "\n";
        return false;
    }
}

// close()
// - Closes and releases the socket safely
// - Resets internal state; context is cleaned up by its destructor
void ZeroMQPublisher::close()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (socket_) {
        try {
            socket_->close();
        }
        catch (const zmq::error_t& e) {
            std::cerr << "ZeroMQPublisher close socket error: " << e.what() << "\n";
        }
        socket_.reset();
    }
    initialized_ = false;
    // context_ will be cleaned up in destructor
}


// -------------------- Subscriber implementation --------------------

// Constructor
// - store connect address and topic filter, create context
ZeroMQSubscriber::ZeroMQSubscriber(const std::string& connectAddress, const std::vector<std::string>& topicFilters)
    : connectAddress_(connectAddress),
    topicFilters_(topicFilters),
    context_(1),
    socket_(nullptr),
    initialized_(false),
    callback_(nullptr),
    thread_(),
    running_(false)
{
}

// Destructor
// - ensure receiving is stopped and resources are released
ZeroMQSubscriber::~ZeroMQSubscriber()
{
    stop();
    close();
}

// init()
// - Create a SUB socket, connect to the given address and set the subscription filter(s)
// - Sets a small receive timeout so the run loop can check the running_ flag periodically
bool ZeroMQSubscriber::init()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_)
        return true;

    try {
        socket_ = std::make_unique<zmq::socket_t>(context_, zmq::socket_type::sub);
        // Do not block forever on close
        int linger = 0;
        socket_->set(zmq::sockopt::linger, linger);

        // Connect to the publisher
        socket_->connect(connectAddress_);

        // Subscribe to the provided topic filters. If none provided, subscribe to everything using empty filter.
        if (topicFilters_.empty()) {
            socket_->set(zmq::sockopt::subscribe, std::string(""));
        }
        else {
            for (const auto& f : topicFilters_) {
                socket_->set(zmq::sockopt::subscribe, f);
            }
        }

        // Set a short receive timeout so the run loop is responsive to stop()
        int rcvTimeoutMs = 100; // milliseconds
        socket_->set(zmq::sockopt::rcvtimeo, rcvTimeoutMs);

        initialized_ = true;
        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQSubscriber init error: " << e.what() << "\n";
        socket_.reset();
        initialized_ = false;
        return false;
    }
}

// start()
// - Starts a background thread that receives messages and invokes the callback
void ZeroMQSubscriber::start(std::function<void(const std::string&, const std::string&)> callback)
{
    if (!callback)
        return;

    // Initialize socket if necessary
    if (!initialized_) {
        if (!init())
            return;
    }

    // If already running, do nothing
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true))
        return;

    callback_ = std::move(callback);
    thread_ = std::thread(&ZeroMQSubscriber::runLoop, this);
}

// stop()
// - Signals the background thread to stop and joins it
void ZeroMQSubscriber::stop()
{
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false))
        return;

    if (thread_.joinable())
        thread_.join();

    // Clear callback after stopping
    callback_ = nullptr;
}

// close()
// - Close and release the socket and reset state
void ZeroMQSubscriber::close()
{
    std::lock_guard<std::mutex> lock(mutex_);
    stop();

    if (socket_) {
        try {
            socket_->close();
        }
        catch (const zmq::error_t& e) {
            std::cerr << "ZeroMQSubscriber close socket error: " << e.what() << "\n";
        }
        socket_.reset();
    }
    initialized_ = false;
}

// runLoop()
// - Background loop that receives multipart messages (topic + message)
// - Uses the short receive timeout set in init() so it can exit promptly when stop() is called
void ZeroMQSubscriber::runLoop()
{
    while (running_.load()) {
        try {
            // Receive topic frame
            zmq::message_t topicMsg;
            auto res = socket_->recv(topicMsg, zmq::recv_flags::none);
            if (!res) {
                // timeout or interrupted, loop back and check running_
                continue;
            }

            std::string topic(static_cast<const char*>(topicMsg.data()), topicMsg.size());

            // Receive message frame
            zmq::message_t msg;
            auto res2 = socket_->recv(msg, zmq::recv_flags::none);
            if (!res2) {
                // incomplete message; skip
                continue;
            }

            std::string message(static_cast<const char*>(msg.data()), msg.size());

            // Invoke callback outside of any locks to avoid deadlocks
            if (callback_) {
                callback_(topic, message);
            }
        }
        catch (const zmq::error_t& e) {
            // EAGAIN indicates no message was available within the timeout
            if (e.num() == EAGAIN) {
                continue;
            }
            std::cerr << "ZeroMQSubscriber receive error: " << e.what() << "\n";
            // In case of other errors, give a small pause to avoid busy-looping
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
