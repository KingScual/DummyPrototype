#include "App.h"
#include "BitStreamConversion.h"

#include <cwchar>
#include <string>
#include <chrono>
#include "Messages.h"
#include "zmq.hpp"

// Constructor: initialize internal handles to null.
// TODO, initialize all of the personal app information here
App::App()
    : m_hInstance(nullptr), 
    m_hWnd(nullptr), 
    m_hButton(nullptr), 
    m_hEdit(nullptr), 
    m_hReceiveEdit(nullptr), 
    outputThread_(),
    m_publisher(nullptr), 
    m_subscriber(nullptr),
    m_appId("LARRY"), 
    m_appRuntimeStart(NULL), 
    m_numToAdd(100), 
    m_numToMultiply(4.5)
{
}

// Destructor: destroy the main window if created and unregister the window class.
App::~App()
{
    // Stop and destroy the subscriber if present
    if (m_subscriber)
    {
        try { m_subscriber->stop(); m_subscriber->close(); }
        catch (...) {}
        m_subscriber.reset();
    }

    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }

    if (m_hInstance)
    {
        UnregisterClassW(m_windowClassName, m_hInstance);
        m_hInstance = nullptr;
    }
    // close the background thread
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false))
        return;

    if(outputThread_.joinable())
    {
        running_ = false;
        outCv_.notify_one();
        outputThread_.join();
    }

    // m_publisher will be cleaned up automatically (its destructor calls close())
}

// Initialize: register the window class, create the main window and child controls (button).
// Returns true on success, false on failure.
bool App::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    m_hInstance = hInstance;
    // set App health status and get the beginning of app running
    m_appHealth = "HEALTHY";
    m_appRuntimeStart = clock();
    running_ = true;

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = App::WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = m_windowClassName;

    if (!RegisterClassExW(&wc))
    {
        return false;
    }

    int width = 800;
    int height = 150;
    DWORD style = WS_OVERLAPPEDWINDOW;

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, style, FALSE);

    m_hWnd = CreateWindowExW(
        0,
        m_windowClassName,
        m_windowTitle,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!m_hWnd)
    {
        UnregisterClassW(m_windowClassName, hInstance);
        return false;
    }

    // Store the pointer to this instance in GWLP_USERDATA so WindowProc can access it if needed
    SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // Get client rect to position controls
    RECT clientRect = {};
    GetClientRect(m_hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    // Create an edit control for text entry (moved down to make room for the label above)
    int topEditX = 10;
    int topEditY = 40;
    int candidateWidth = clientWidth - 160;
    int topEditWidth = (candidateWidth > 200) ? candidateWidth : 200;
    m_hEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        nullptr,
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        topEditX, topEditY,
        topEditWidth, 24,
        m_hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(EDIT_ID)),
        hInstance,
        nullptr);

    // Create a button to submit the text from the edit control (moved down to align with the edit)
    int buttonX = topEditX + topEditWidth + 10;
    m_hButton = CreateWindowExW(
        0,
        L"BUTTON",
        L"Submit",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        buttonX, topEditY - 2,
        120, 30,
        m_hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(BUTTON_ID)),
        hInstance,
        nullptr);

    // Create a receive-only edit control at the bottom for data from another application
    int recvHeight = 24;
    int recvY = clientHeight - recvHeight - 10;
    int recvWidthCandidate = clientWidth - 20;
    int recvWidth = (recvWidthCandidate > 200) ? recvWidthCandidate : 200;
    m_hReceiveEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        nullptr,
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        10, recvY,
        recvWidth, recvHeight,
        m_hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(RECEIVE_EDIT_ID)),
        hInstance,
        nullptr);

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    // Initialize ZeroMQ publisher to connect to the proxy frontend socket
    // Initialize ZeroMQ publisher to send messages when the button is clicked.
    try {
        m_publisher = std::make_unique<ZeroMQPublisher>(PROXYFRONTEND);
        if (!m_publisher->init()) {
            // Initialization failed; keep the pointer so publish() can attempt init lazily.
            OutputDebugStringA("ZeroMQ publisher init failed\n");
        }
    }
    catch (const std::exception& ex) {
        // If ZeroMQ or allocation throws, log but continue running the UI
        OutputDebugStringA(ex.what());
    }

    // Initialize ZeroMQ subscriber to connect to the proxy backend socket for messages in background and post to UI
    try {
        // Connect to the same multicast group; subscribe to all work topics and response topics from Dummy2 and Dummy3
        m_subscriber = std::make_unique<ZeroMQSubscriber>(PROXYBACKEND, std::vector<std::string>{
            "appstatus", 
            "appdatarequest1", 
            "appdatarequest2",
            "dummyService2",
            "dummyService3"}); // subscribe to Dummy2 and Dummy3
        if (m_subscriber->init()) {
            // start receiving; callback will post WM_ZMQ_MESSAGE to UI thread
            m_subscriber->start([this](const std::string& topic, const std::string& message) {
                // Convert message (UTF-8) to wide string
                // checking that we have data
                int wlen = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
                if (wlen > 0) {
                    wchar_t* wbuf = reinterpret_cast<wchar_t*>(GlobalAlloc(GMEM_FIXED, wlen * sizeof(wchar_t)));
                    if (wbuf) {
                        MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, wbuf, wlen);
                        // Post to UI thread; WindowProc will free the buffer
                        if (m_hWnd) {
                            PostMessageW(m_hWnd, WM_ZMQ_MESSAGE, 0, reinterpret_cast<LPARAM>(wbuf));
                        }
                        else {
                            GlobalFree(wbuf);
                        }
                    }
                }
                });
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

// Run: main message loop. Processes messages until WM_QUIT is received and returns exit code.
int App::Run()
{
    MSG msg;
    outputThread_ = std::thread(&App::OutputThread, this);
    CreateConsoleWindow();

    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

// WindowProc: static window procedure. Dispatches messages to the App instance stored in
// GWLP_USERDATA and handles WM_COMMAND (button clicks), WM_PAINT and WM_DESTROY.
LRESULT CALLBACK App::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
    {
        // Reposition controls when the window is resized
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);
        App* pThis = reinterpret_cast<App*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        if (pThis)
        {
            // Top edit and button
            int topEditX = 10;
            int topEditY = 40;
            int candidateWidth = newWidth - 160;
            int topEditWidth = (candidateWidth > 200) ? candidateWidth : 200;
            if (pThis->m_hEdit)
                MoveWindow(pThis->m_hEdit, topEditX, topEditY, topEditWidth, 24, TRUE);

            int buttonX = topEditX + topEditWidth + 10;
            if (pThis->m_hButton)
                MoveWindow(pThis->m_hButton, buttonX, topEditY - 2, 120, 30, TRUE);

            // Receive edit at bottom
            int recvHeight = 24;
            int recvY = newHeight - recvHeight - 10;
            int recvWidthCandidate = newWidth - 20;
            int recvWidth = (recvWidthCandidate > 200) ? recvWidthCandidate : 200;
            if (pThis->m_hReceiveEdit)
                MoveWindow(pThis->m_hReceiveEdit, 10, recvY, recvWidth, recvHeight, TRUE);
        }
        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        App* pThis = reinterpret_cast<App*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        if (pThis)
        {
            if (id == BUTTON_ID && code == BN_CLICKED)
            {
                pThis->OnButtonClicked();
                return 0;
            }
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // Draw the label above the top edit control
        const wchar_t* text = L"Messages to send are appstatus, appdatarequest1, or appdatarequest2. Enter below and press Submit";
        TextOutW(hdc, 10, 12, text, static_cast<int>(std::wcslen(text)));

        // Draw a label above the receive-only edit control at the bottom
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int clientHeight = clientRect.bottom - clientRect.top;
        int recvHeight = 24;
        int recvY = clientHeight - recvHeight - 10;
        int labelY = recvY - 18; // place the label a bit above the receive control
        const wchar_t* recvLabel = L"Received data (from other applications):";
        TextOutW(hdc, 10, labelY, recvLabel, static_cast<int>(std::wcslen(recvLabel)));

        EndPaint(hWnd, &ps);
        return 0;
    }

    }

    // Handle ZMQ message posted from receiver thread
    if (uMsg == WM_ZMQ_MESSAGE)
    {
        App* pThis = reinterpret_cast<App*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        if (pThis)
        {
            // incoming is the payload, can manipulated here
            wchar_t* incoming = reinterpret_cast<wchar_t*>(lParam); // can this just be a string
            
            if (incoming)
            {
                pThis->DoWork(incoming);
                //pThis->SetReceivedText(incoming);
                //for right now, just say that a message was recieved, already outputting from ZeroMQ.cpp
                
                // convert from string to wchar_t*
                std::string rcv = "Message Received";
                int size = MultiByteToWideChar(CP_UTF8, 0, rcv.c_str(), -1, nullptr, 0);
                std::wstring ws(size, 0);
                MultiByteToWideChar(CP_UTF8, 0, rcv.c_str(), -1, &ws[0], size);
                const wchar_t* w = ws.c_str();
                // let user know that we recieved something
                pThis->SetReceivedText(w);

               
                GlobalFree(incoming);
            }
        }
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// OnButtonClicked: invoked when the button is clicked. Reads text from the edit control and
// displays it in a message box.
// process the buffer to determine what message to send
void App::OnButtonClicked()
{
    if (!m_hEdit)
        return;

    const int bufSize = 1024;
    wchar_t buffer[bufSize] = {};
    int len = GetWindowTextW(m_hEdit, buffer, bufSize);

   
    if(len > 0){    
        
        // Convert wide char (UTF-16) buffer to UTF-8 std::string for ZeroMQ
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, buffer, len, nullptr, 0, nullptr, nullptr);
        std::string msg;
        if (utf8Len > 0) {
            msg.resize(utf8Len);
            WideCharToMultiByte(CP_UTF8, 0, buffer, len, &msg[0], utf8Len, nullptr, nullptr);

            // Determine the payload to publish based on user input
            // this could be a function
            // MAKE THIS A REQUEST FOR DATA
            if (msg == "appstatus"){
                
                AppStatus appStatus;

                //appStatus.appId = m_appId;
                //appStatus.appHealth = DetermineAppHealth();
                appStatus.appRuntime = GetAppRunningTime();

                PublishAndDisplay(msg , &appStatus);
            }
            else if (msg == "appdatarequest1"){
            
                AppDataRequest1 additionRequest;

                //additionRequest.appId = m_appId;
                //additionRequest.appHealth = DetermineAppHealth();
                additionRequest.numberToAdd = m_numToAdd;

                PublishAndDisplay(msg, &additionRequest);
            }
            else if (msg == "appdatarequest2"){
            
                AppDataRequest2 multiplicationRequest;

                //multiplicationRequest.appId = m_appId;                
                //multiplicationRequest.appHealth = DetermineAppHealth();
                multiplicationRequest.numberToMultiply = m_numToMultiply;

                PublishAndDisplay(msg, &multiplicationRequest);
            }
            else
            {
                MessageBoxW(m_hWnd, L"Please enter the correct text from above.", L"Error", MB_OK | MB_ICONERROR);
            }            
        }        
    }
    else
    {
        MessageBoxW(m_hWnd, L"No text entered.", L"Info", MB_OK | MB_ICONINFORMATION);
    }

}

// SetReceivedText: set the text shown in the receive-only edit control (used by other apps to send data).
void App::SetReceivedText(const wchar_t* text)
{
    SetWindowTextW(m_hReceiveEdit, text ? text : L"");
}

// wWinMain: application entry point. Creates the App instance, initializes it, and runs the message loop.
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PWSTR /*pCmdLine*/, int nCmdShow)
{
    App app;

    if (!app.Initialize(hInstance, nCmdShow))
    {
        return -1;
    }

    return app.Run();
}

double App::GetAppRunningTime()
{
    clock_t now = clock();
    double time = double(now - m_appRuntimeStart) / CLOCKS_PER_SEC;
    return time;
}

template <typename T>
void App::PublishAndDisplay(const std::string topic, T object) {

    // Publish using ZeroMQ publisher if available
    if (m_publisher) {

        //bool published = m_publisher->publish("Dummy1", str); PART OF BITSTREAM FIX

        bool published = m_publisher->publish(topic, object);
        if (published) {
            MessageBoxW(m_hWnd, L"Message published successfully.", L"Info", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBoxW(m_hWnd, L"Failed to publish message.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}


std::string App::DetermineAppHealth() {

    double currentAppRuntime = GetAppRunningTime();
    if (GetAppRunningTime() < 120.0000000)
    {
        m_appHealth = "HEALTHY";
    }
    else if (GetAppRunningTime() < 240.000000)
    {
        m_appHealth = "IMPACTED";
    }
    else if (GetAppRunningTime() < 360.000000) 
    {
        m_appHealth = "SEVERELY DEGRADED";
    }

    return m_appHealth;
}
;

 void App::DoWork(const wchar_t* text)
{
    //take in a topic from the sub'r, determine what to do 1. cout the ack from the recv'ing services repsonse, 2.publish a struct based on request and cout
    //publish a response from the app with the appname and the requested data
    // 
     //convert text to a string for simplicity
     int size = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
     std::string topic = {};
     WideCharToMultiByte(CP_UTF8, 0, text, -1, &topic[0], size, nullptr, nullptr);

    

    // based on the topic, select which struct is to be filled and sent over
    // if the repsonse topic is the other dummy services, do nothing but cout the object they sent
    // publish back with dummy1
     std::string responseTopic = "dummyService1";
     std::string output = {};

    if (topic == "appstatus") {

        AppStatus A;
        //std::memcpy(&A, msg.data(), sizeof(AppStatus));

        A.appRuntime = GetAppRunningTime();
        PublishAndDisplay(responseTopic, &A);
        // print out the data so the user can verify?
        // we recv a struct and then we "do work", i.e. outputting the member variables
        // shouldn't this move to the app itself?
        // i.e. Subscriber recv's a payload and then needs to pass said data to the app ("worker")
        //std::cout << "Sent Message has id: " << A.appId << std::endl;
        //std::cout << "Sent Message has health " << A.appHealth << std::endl;
        output = "Sent Message has runtime of " + std::to_string(A.appRuntime);
    }
    else if (topic == "appdatarequest1") {

        AppDataRequest1 A;
        //std::memcpy(&A, msg.data(), sizeof(AppDataRequest1));
        A.numberToAdd = m_numToAdd;
        PublishAndDisplay(responseTopic, &A);
        //std::cout << "Sent Message has id " << A.appId << std::endl;
        //std::cout << "Sent Message has health " << A.appHealth << std::endl;
        output = "Sent Message has number " + std::to_string(A.numberToAdd);
    }
    else if (topic == "appdatarequest2") {

        AppDataRequest2 A;
        //std::memcpy(&A, msg.data(), sizeof(AppDataRequest2));

        A.numberToMultiply = m_numToMultiply;
        PublishAndDisplay(responseTopic, &A);
        //std::cout << "Sent Message has id: " << A.appId << std::endl;
        //std::cout << "Sent Message has health " << A.appHealth << std::endl;
        output = "Sent Message has multiplication of " + std::to_string(A.numberToMultiply);
        
    }
    AsyncPrint(output);

    // if the rec'd message from publisher was a response topic, just cout and don't publish
    // is it okay to have a duplicate here? because 1 is subscribed to a message that 1 also sends (because we
    // request it from 2 and 3, 1's sub'r will also report it's own data to be cout'ed
    // it is okay in this case b/c our Messages are quite vague for the sake of demonstration

    if (topic == "dummyService1") {
        //cout dummyService 1's stuff
    };
    if (topic == "dummyService2") {
        //cout dummyService 2's stuff
    };
    if (topic == "dummyService3") {
        //cout dummyService 3's stuff
    };


   
    
 
}

 void App::OutputThread() {

     while (running_) {
         std::unique_lock <std::mutex> lock(outMutex_);
         outCv_.wait(lock, [this] {return !outQueue_.empty() || !running_; });

         while (!outQueue_.empty()) {
             std::cout << outQueue_.front() << std::endl;
             outQueue_.pop();
         }
     }

 }

 void App::AsyncPrint(const std::string& msg) {
     {
         std::lock_guard <std::mutex> lock(outMutex_);
         outQueue_.push(msg);
     }
     outCv_.notify_one();
 }

 void App::CreateConsoleWindow() {

     AllocConsole();
     FILE* fp = nullptr;
     freopen_s(&fp, "CONOUT$", "w", stdout);
     freopen_s(&fp, "CONOUT$", "w",stderr);
     freopen_s(&fp, "CONIN$", "r", stdin);

     std::ios::sync_with_stdio(true);

 }

