// DummyBITService.cpp : Defines the entry point for the application.
//
#include <iostream>

#include "framework.h"
#include "DummyBITService.h"
#include "DummyBitService_Subscriber.h"

#define MAX_LOADSTRING 100
#define ID_BUTTON_GETSTATUS 1001
#define ID_EDIT_STATUS 1002

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hEditStatusInit = nullptr;                 // status text box
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Global Proxy Variables
const std::string PROXYFRONTEND = "tcp://localhost:5557";
const std::string PROXYBACKEND = "tcp://localhost:5558";

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DummyBITService_Subscriber subscriber;
LPCWSTR statusInitText;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize Subscriber
     if (subscriber.Initialize())
    {
        OutputDebugString(L"Subscriber Created\n");
    }
    else
    {
        OutputDebugString(L"Subscriber Failed to Create\n");
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DUMMYBITSERVICE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DUMMYBITSERVICE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUMMYBITSERVICE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DUMMYBITSERVICE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

  // Create the main application window using the previously registered class.
   // CW_USEDEFAULT lets Windows pick a reasonable default position/size.
   HWND hWnd = CreateWindowW(
       szWindowClass,    // class name
       szTitle,          // window title
       WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0,
	   600, 200,        // width, height
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

    // Create "StatusInit" text:
   // - Parent is the main window (hWnd).
   // - Styles: WS_BORDER, WS_VISIBLE, WS_CHILD.
   // - Command ID is ID_BUTTON_GETSTATUS; clicks generate WM_COMMAND with this id.
   CreateWindowW(
       L"STATIC", L"  statusInit",
       WS_BORDER | WS_VISIBLE | WS_CHILD,
       10, 10, 80, 20,
       hWnd, (HMENU)ID_BUTTON_GETSTATUS, hInstance, nullptr);

   // Create read-only edit control to display status:
   // - WS_EX_CLIENTEDGE gives a sunken border to look like a status field.
   // - ES_READONLY prevents user editing (program updates it via SetWindowTextW).
   // - Keep a global HWND (hEditStatusInit) so the button handler can update text.
   hEditStatusInit = CreateWindowExW(
       WS_EX_CLIENTEDGE, L"EDIT", L"",
       WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
       100, 10, 50, 20,
       hWnd, (HMENU)ID_EDIT_STATUS, hInstance, nullptr);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // Update output on main window
            if (subscriber.GetStatReq() == 1) {
                statusInitText = L"1";
            }
            else {
                statusInitText = L"0";
            }
            SetWindowTextW(hEditStatusInit, statusInitText);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
