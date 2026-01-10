#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

#define ZMQ_BUILD_DRAFT_API
// Forward include for cppzmq
#include <zmq.hpp>

class ZeroMQPublisher
{
public:
    // bindAddress
    explicit ZeroMQPublisher(const std::string& bindAddress = "tcp://*:5556");
    ~ZeroMQPublisher();

    // Initialize and bind the publisher socket. Returns true on success.
    bool init();

    // Publish a message under a topic. Topic and message are plain strings.
    // Returns true on success, false on failure.
    bool publish(const std::string& topic, const std::string& message);

    // Close the socket and context.
    void close();

private:
    std::string bindAddress_;
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::mutex mutex_;
    bool initialized_;
};

// Simple ZeroMQ subscriber helper that receives messages on a background thread
class ZeroMQSubscriber
{
public:
    // connectAddress
    // topicFilters example: empty vector subscribes to everything, or a list of topics to receive only those
    explicit ZeroMQSubscriber(const std::string& connectAddress = "tcp://localhost:5556",
        const std::vector<std::string>& topicFilters = {});
    ~ZeroMQSubscriber();
    
    // Initialize and connect the subscriber socket. Returns true on success.
    bool init();

    // Start background receiving. The callback will be invoked for each message as (topic, message).
    void start(std::function<void(const std::string&, const std::string&)> callback);

    // Stop receiving and join the background thread.
    void stop();

    // Close subscriber socket and context.
    void close();

private:
    void runLoop();

    std::string connectAddress_;
    std::vector<std::string> topicFilters_;
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::mutex mutex_;
    bool initialized_;

    std::function<void(const std::string&, const std::string&)> callback_;
    std::thread thread_;
    std::atomic<bool> running_;
};
