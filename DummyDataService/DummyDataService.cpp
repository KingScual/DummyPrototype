// DummyDataService.cpp : Defines the entry point for the application.
//
// This is a minimal Windows desktop application that creates a main window
// with a "Get Status" button and a read-only status text box.  The user
// presses the button to request a status; the application demonstrates
// updating the status control with a placeholder string.
//
// Notes:
// - Uses the Win32 API directly (CreateWindowW, WndProc message loop, etc.).
// - Keeps UI logic synchronous and single-threaded for simplicity.
// - Replace the placeholder status retrieval in the button handler with
//   real logic as needed.

#include "framework.h"
#include "DummyDataService.h"

#define MAX_LOADSTRING 100
#define ID_BUTTON_GETSTATUS 1001
#define ID_EDIT_STATUS 1002

// Global Variables:
//
// hInst
//   - The HINSTANCE for the running process. Stored in InitInstance and used
//     when creating dialogs and controls.
//
// hEditStatus
//   - HWND of the read-only edit control used to display the status string.
//     Kept global so the WndProc handler can update it when the button is clicked.
//
// szTitle / szWindowClass
//   - Buffers that hold the application title and window class name. Populated
//     from resources at startup.
HINSTANCE hInst;                                // current instance
HWND hEditStatus = nullptr;                     // status text box
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Load string resources for the window title and class name. These are
    // defined in the resource script (e.g. .rc file) and localized if needed.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DUMMYDATASERVICE, szWindowClass, MAX_LOADSTRING);

    // Register the window class before creating any windows that use it.
    MyRegisterClass(hInstance);

    // Create and show the main application window.
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Load accelerators (keyboard shortcuts) if defined in resources.
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DUMMYDATASERVICE));

    MSG msg;

    // Standard Win32 message loop:
    // - GetMessage retrieves messages from the thread message queue.
    // - TranslateAccelerator checks for and translates keyboard accelerators.
    // - TranslateMessage / DispatchMessage dispatch to WndProc.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Return the exit code supplied to PostQuitMessage.
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  This function fills a WNDCLASSEXW structure describing the window class
//  and then calls RegisterClassExW so CreateWindow(Ex) can create windows
//  of this class later. Important fields:
//   - lpfnWndProc: pointer to the window procedure (WndProc).
//   - hbrBackground: background brush used to clear the window.
//   - lpszMenuName: optional menu resource name (if the app has a menu).
//   - lpszClassName: the class name used by CreateWindowW.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    // Class styles: redraw on horizontal/vertical resize.
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;                  // main window procedure
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    // Load icons/cursors from the application's resources (fallbacks if needed).
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUMMYDATASERVICE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    // Use a standard window background color.
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    // Optional menu resource name (from resource file) - kept for compatibility.
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DUMMYDATASERVICE);
    // Class name must match the string used with CreateWindowW.
    wcex.lpszClassName  = szWindowClass;
    // Small icon for the window.
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register the class and return its ATOM. Applications should check the
    // return value in production code and handle registration failures.
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//   - Stores the instance handle in the global `hInst` variable so other
//     functions can create dialogs and controls using the same instance.
//   - Creates the main overlapped window and two child controls:
//       * A push button labeled "Get Status".
//       * A read-only edit control used to display the status string.
//   - Shows the window and updates it.
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

   // Create "Get Status" button:
   // - Parent is the main window (hWnd).
   // - Styles: WS_TABSTOP (tab navigation), WS_VISIBLE, WS_CHILD, BS_PUSHBUTTON.
   // - Command ID is ID_BUTTON_GETSTATUS; clicks generate WM_COMMAND with this id.
   CreateWindowW(
       L"BUTTON", L"Get Status",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
       10, 10, 100, 30,
       hWnd, (HMENU)ID_BUTTON_GETSTATUS, hInstance, nullptr);

   // Create read-only edit control to display status:
   // - WS_EX_CLIENTEDGE gives a sunken border to look like a status field.
   // - ES_READONLY prevents user editing (program updates it via SetWindowTextW).
   // - Keep a global HWND (hEditStatus) so the button handler can update text.
   hEditStatus = CreateWindowExW(
       WS_EX_CLIENTEDGE, L"EDIT", L"",
       WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
       120, 10, 300, 25,
       hWnd, (HMENU)ID_EDIT_STATUS, hInstance, nullptr);

   // Show and refresh the window so it appears on screen.
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  Message handling overview:
//   - WM_COMMAND: handle menu selections and control notifications (button clicks).
//   - WM_PAINT: repaint the client area using BeginPaint/EndPaint.
//   - WM_DESTROY: post a quit message to end the message loop.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            // LOWORD(wParam) contains the control or menu ID.
            int wmId = LOWORD(wParam);

            // Parse menu selections and control commands:
            switch (wmId)
            {
            case IDM_ABOUT:
                // Open the About dialog. DialogBox is modal and uses the dialog proc `About`.
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                // User chose Exit from a menu; destroy the main window to quit.
                DestroyWindow(hWnd);
                break;
            case ID_BUTTON_GETSTATUS:
                {
                    // Handler for the "Get Status" button.
                    // Replace the static status string below with real status retrieval
                    // logic (e.g., query a service, check a subsystem, etc.).
                    const wchar_t* statusText = L"Status: OK";

                    // If the edit control was created successfully, set its text so the
                    // user sees the status. If not, fall back to a message box.
                    if (hEditStatus)
                    {
                        SetWindowTextW(hEditStatus, statusText);
                    }
                    else
                    {
                        // Fallback for robustness.
                        MessageBoxW(hWnd, statusText, L"Service Status", MB_OK | MB_ICONINFORMATION);
                    }
                }
                break;
            default:
                // For any unhandled WM_COMMAND values, let DefWindowProc handle them.
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            // Standard painting sequence. If no custom drawing is required,
            // just call BeginPaint/EndPaint to validate the update region.
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any custom drawing code that uses `hdc` here.
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        // PostQuitMessage posts WM_QUIT which causes GetMessage to return 0 and
        // ends the main message loop in wWinMain.
        PostQuitMessage(0);
        break;

    default:
        // Default handling for messages the application does not process.
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    // Return value is message-specific; 0 indicates we've processed the message.
    return 0;
}

// Message handler for the About dialog box.
//
// The About dialog is defined in the resources (dialog template). This
// callback processes messages sent to the dialog. For this simple example
// it only handles initialization and OK/Cancel button presses.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        // Initialize dialog controls here if needed.
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        // End the dialog when the user clicks OK or Cancel.
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}
