#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <atomic>
// Forward declare or include ZeroMQ publisher helper
#include "ZeroMQ.h"
// Include of Proxy port constants for Pubs/Subs connections
#include "Proxy.h"

// Simple application class that wraps a Win32 window and a button.
class App
{
public:
    // Construct the App object and initialize member variables.
    App();

    // Destroy the main window and unregister the window class.
    ~App();

    // Initialize the Win32 window and controls. Returns true on success.
    bool Initialize(HINSTANCE hInstance, int nCmdShow);

    // Run the main message loop. Returns the application's exit code.
    int Run();

    // Static window procedure used by the Win32 API to dispatch messages to the App instance.
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Output to the window that something was sent
    void SetReceivedText(const wchar_t* text);

    // take in a queue of topics and payloads to work on and remove them 
    // from the queue after work is done
    void DoWork(std::queue <std::pair<std::string, void*>>& work);

    // Calculates the amount of time the app has been running since initialization
    double GetAppRunningTime();

    // Take in any object or message struct to publish via ZeroMQ and output  box text
    template <typename T>
    void PublishAndDisplay(const std::string topic, T object);

    // Determine app health based on app running time
    std::string DetermineAppHealth();

    // Takes enque'd cout messages off the queue and outputs them to console
    void OutputThread();

    // Method for putting cout messages onto the queue
    void AsyncPrint(const std::string& msg);

    // Creates a console window that the background output message thread can write into
    void CreateConsoleWindow();

    // Custom Windows message posted when a ZMQ message arrives
    static const UINT WM_ZMQ_MESSAGE = WM_APP + 1;

private:
    HINSTANCE m_hInstance; // Application instance handle
    HWND m_hWnd;           // Main window handle
    HWND m_hButton;        // Button control handle
    HWND m_hEdit;          // Edit control handle for user text entry
    HWND m_hReceiveEdit;   // Edit control used to display received data
    
    std::queue<std::string> outQueue_; // Queue for holding output (console) messages
    std::mutex outMutex_;              // lock for output thread
    std::condition_variable outCv_;     
    std::atomic<bool> running_;   
    std::thread outputThread_;

    std::queue <std::pair<std::string, void*>> workQueue; // Queue for storing topics and payloads to work on
    bool m_iHaveWorkToDo;                                 // flag for the app to know that there is work to be done

    // data that each App has to be initialized at runtime and requested from other apps
    const std::string m_appId;
    std::string m_appHealth;
    clock_t m_appRuntimeStart;
    const uint32_t m_numToAdd;
    const float m_numToMultiply;

    // place to save the sent topic / payload combo from PUB'R and SUB'R in ZeroMQ lib
    std::string m_topic;
    void* m_payload;
    // saves off the context of the reponse asked for to determine what message to build in the asking app
    std::string m_reponseContext;

    static const int BUTTON_ID = 1001; // Identifier for the button control
    static const int EDIT_ID = 1002;   // Identifier for the edit control (not strictly required)
    static const int RECEIVE_EDIT_ID = 1003; // Identifier for the receive edit control

    // Handler invoked when the button is clicked.
    void OnButtonClicked();

    const wchar_t* m_windowClassName = L"BasicAppWindowClass";
    const wchar_t* m_windowTitle = L"Dummy Service 1";

    // ZeroMQ publisher used to send messages when the button is clicked
    std::unique_ptr<ZeroMQPublisher> m_publisher;

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
};
