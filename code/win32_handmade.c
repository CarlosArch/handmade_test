#include <windows.h>

#define internal static
#define persistent static

/* Prototypes */
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT main_window_behavior(HWND, UINT, WPARAM, LPARAM);
internal void resize_bitmap_section(void);

/* Functions */
int WinMain(
        HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR command_line,
        int showcode)
{
    { // Window Creation
        WNDCLASSEX main_window_class = {0};
        { // Definition
            main_window_class.cbSize = sizeof(WNDCLASSEX);
            main_window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            main_window_class.lpfnWndProc = main_window_behavior;
            main_window_class.hInstance = instance;
            main_window_class.lpszClassName = "main_window";
            main_window_class.hIcon         = 0; // TODO: Add icons
            main_window_class.hIconSm       = 0; // TODO: Add icons

            main_window_class.cbClsExtra = 0;
            main_window_class.cbWndExtra = 0;
            main_window_class.hCursor = 0;
            main_window_class.hbrBackground = 0;
            main_window_class.lpszMenuName = 0;
        }

        RegisterClassExA(&main_window_class);

        CreateWindowExA(
            0,                                // ExStyle
            main_window_class.lpszClassName,  // Class Name
            "Handmade Tutorial",              // Window Name
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window Style
            CW_USEDEFAULT,                    // x
            CW_USEDEFAULT,                    // y
            CW_USEDEFAULT,                    // width
            CW_USEDEFAULT,                    // height
            0,                                // Parent
            0,                                // Menu
            instance,                         // Instance
            0                                 // Extra Parameter
        );
    }

    { // Message Handling Loop
        MSG message;
        while (GetMessageA(&message, 0, 0, 0) > 0)
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }

    return 0;
}

LRESULT main_window_behavior(
        HWND window,
        UINT message,
        WPARAM w_param,
        LPARAM l_param)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_CLOSE:
            OutputDebugStringA("Closing Window.\n");
            DestroyWindow(window);
            break;

        case WM_DESTROY:
            OutputDebugStringA("Destroying Window.\n");
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            {
                OutputDebugStringA("Rezising Window.\n");
            } break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC context = BeginPaint(window, &paint);
            { // Paint process.
                int x = paint.rcPaint.left;
                int y = paint.rcPaint.top;
                int w = paint.rcPaint.right - paint.rcPaint.left;
                int h = paint.rcPaint.bottom - paint.rcPaint.top;
                PatBlt(context, x, y, w, h, WHITENESS);
            }
            EndPaint(window, &paint);
        } break;

        default:
            result = DefWindowProcA(window, message, w_param, l_param);
            break;
    }
    return result;
}

// WinMain defines the program's entry point.
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain

// Windows:
    // WNDCLASSEX means window class, contains info on how to create the window.
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
    // Window Style.
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
    // Window procedure, function called when window events happen.
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callwindowproca
    // Registering class after it's defined:
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexa

    // Window Procedure: handles window events.
    // https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)

    // Creating the window
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa

// MessageBox(
//     0, "Mensaje", "Titulo",
//     MB_OK|MB_ICONINFORMATION);