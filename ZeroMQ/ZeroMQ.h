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

    // Publish a message under a topic, message can be any type as defined in Messages.h.
    // Returns true on success, false on failure.
    // Calls serialize() to encode the payload if necessary
    // OVERLOAD PUBLISH FOR EACH STRUCT TYPE
    bool publish(const std::string& topic);
    bool publish(const std::string& topic, const AppStatus& message);
    bool publish(const std::string& topic, const AppDataRequest1& message);
    bool publish(const std::string& topic, const AppDataRequest2& message);

    // NOTE: technically, it is better to use ProtoBuffer or FlatBuffer to serialize
    //       rather than doing it by hand, but I don't want to have to download one
    //       more library and frustrate IT and prolong this project.
    //       overloaded for each struct/message type. writes and packs all data members

    std::string serialize(const AppStatus& message);
    std::string serialize(const AppDataRequest1& message);
    std::string serialize(const AppDataRequest2& message);

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
    // Overload to let callback return whatever type of struct got sent
    void start(std::function<void(const std::string&, std::unique_ptr<Message>)>callback);

    // Stop receiving and join the background thread.
    void stop();

    // Close subscriber socket and context.
    void close();

   // deSerialize()
    AppStatus deserializeStatus(const std::string& s);
    AppDataRequest1 deserializeAddition(const std::string& s);
    AppDataRequest2 deserializeMultiplication(const std::string& s);

    // helper function to make response or request logic in subscriber much clearer
    // considers all available topics and boils down the rec'd ZeroMQ message to
    // "this was a request from an app to other apps, and this was a response"
    // still need to return a string if it was a response to acertain the struct object to send
    std::string determineRequestOrResponse(const std::string& topic);

private:
    void runLoop();

    std::string connectAddress_;
    std::vector<std::string> topicFilters_;
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::mutex mutex_;
    bool initialized_;

    std::function<void(const std::string&, std::unique_ptr<Message>)> callback_;
    std::thread thread_;
    std::atomic<bool> running_;
};



