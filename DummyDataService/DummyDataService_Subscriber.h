#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>

/*
// Forward declare or include ZeroMQ publisher helper
#include "C:\DummyServicePrototype\DummyPrototype\ZeroMQ\ZeroMQ.h"
// Include of Proxy port constants for Pubs/Subs connections
#include "C:\DummyServicePrototype\DummyPrototype\Proxy\Proxy\Proxy.h"

class DummyDataService_Subscriber
{
public:
    // Construct the DummyDataService_Subscriber object and initialize member variables.
    DummyDataService_Subscriber();

    // Destroy the main window and unregister the window class.
    ~DummyDataService_Subscriber();

    // Initialize Subscriber. Return True on success.
    bool Initialize();

    // Custom Windows message posted when a ZMQ message arrives
    static const UINT WM_ZMQ_MESSAGE = WM_APP + 1;

private:
  
    HWND m_hReceiveEdit;   // Edit control used to display received data

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
};*/