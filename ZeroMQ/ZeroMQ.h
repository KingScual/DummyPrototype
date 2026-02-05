#pragma once
#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <cerrno>
#include "iostream"
#include "Messages.h"

// Forward include for cppzmq
#define ZMQ_BUILD_DRAFT_API
#include <zmq.hpp>


class ZeroMQPublisher
{
public:
    // bindAddress example: "tcp://*:5556"
    explicit ZeroMQPublisher(const std::string& connectAddress = ""); // empty to be specified upon declaration
    ~ZeroMQPublisher();

    // Initialize and bind the publisher socket. Returns true on success.
    bool init();

    // Publish a message under a topic. Topic and message are plain strings.
    // Returns true on success, false on failure.
    template <typename T> 
    bool publish(const std::string& topic, const T message);

    // Close the socket and context.
    void close();

private:
    std::string connectAddress_; // using a proxy to connect, so we don't bind the pub, just connect
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::mutex mutex_;
    bool initialized_;
};

// Simple ZeroMQ subscriber helper that receives messages on a background thread
class ZeroMQSubscriber
{
public:
    // connectAddress example: "tcp://localhost:5556"
    // topicFilters example: empty vector subscribes to everything, or a list of topics to receive only those
    explicit ZeroMQSubscriber(const std::string& connectAddress = "", // empty to be specified upon declaration
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

// publish(topic, message)
// - Ensures the socket is initialized, then sends a multipart message:
//   first frame = topic, second frame = message
// - Uses a mutex to make publishing thread-safe
// - Any ZMQ errors are caught and logged

/* **** NOTE****
*  zmq_send() can TECHNICALLY send raw structs with no checks for bit padding/endianness/
*  any manual serialization.
*
*  We are going to assume this in our demo as
*  we are sending messages to the SAME language and technically
*  the box WILL be a tightly controlled system.
*
*  However, this code is NOT robust, i.e. we're talking to another system made in another language
*  / made with another compiler / the structs aren't the same etc. We are making MANY assumptions here.
*
*  To make more robust, we'd be using zmq_msg_t <messagename> / zmq_msg_send() & recv() / and zmq_msg_close()
*  variants.
*/
template <typename T>
inline bool ZeroMQPublisher::publish(const std::string& topic, const T message)
{
    // Ensure socket is initialized
    if (!initialized_) {
        if (!init())
            return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    try {
        // copy the read in payload into a zmq::message to send after topic
        zmq::message_t payload(sizeof(T));
        std::memcpy(payload.data(), message, sizeof(T));

        // Send topic as first frame
        zmq::const_buffer topicBuf(topic.data(), topic.size());
        socket_->send(topicBuf, zmq::send_flags::sndmore);

        // Send message as second frame
        // still using zmq_send() under the hood, we rather need
        // a zmq::msg_t for zmq_msg_send() for complex struct payloads
        // const_buffer might not work for user-defined types
        //zmq::const_buffer msgBuf(&message, sizeof(message);;;
        //zmq_send(socket_, message, sizeof(message), 0);
        // do we need a zmq_msg_close(&msgBuf) since we'd need to use zmq_msg_send(&msgBuf)?
        socket_->send(payload, zmq::send_flags::none);

        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQPublisher publish error: " << e.what() << "\n";
        return false;
    }
}


