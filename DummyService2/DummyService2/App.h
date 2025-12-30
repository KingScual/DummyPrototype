#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <memory>

// Forward declare or include ZeroMQ publisher helper
#include "ZeroMQ.h"

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

    // Set text in the receive box (used to receive data from another application).
    void SetReceivedText(const wchar_t* text);

    // Custom Windows message posted when a ZMQ message arrives
    static const UINT WM_ZMQ_MESSAGE = WM_APP + 1;

private:
    HINSTANCE m_hInstance; // Application instance handle
    HWND m_hWnd;           // Main window handle
    HWND m_hButton;        // Button control handle
    HWND m_hEdit;          // Edit control handle for user text entry
    HWND m_hReceiveEdit;   // Edit control used to display received data

    static const int BUTTON_ID = 1001; // Identifier for the button control
    static const int EDIT_ID = 1002;   // Identifier for the edit control (not strictly required)
    static const int RECEIVE_EDIT_ID = 1003; // Identifier for the receive edit control

    // Handler invoked when the button is clicked.
    void OnButtonClicked();

    const wchar_t* m_windowClassName = L"BasicAppWindowClass";
    const wchar_t* m_windowTitle = L"Dummy Service 2";

    // ZeroMQ publisher used to send messages when the button is clicked
    std::unique_ptr<ZeroMQPublisher> m_publisher;

    // ZeroMQ subscriber used to receive messages in the background
    std::unique_ptr<ZeroMQSubscriber> m_subscriber;
};
