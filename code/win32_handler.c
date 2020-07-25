#include <windows.h>
#include "my_definitions.h"
#include "win32_handler.h"

global uint8 quit;
global MY_WINDOW window;
global MY_BITMAP bitmap;

int WinMain(
        HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR     command_line,
        int       showcode)
{
    { // Window Creation
        window.w_class = (WNDCLASSEXA) {
            /* cbSize */        sizeof(WNDCLASSEXA),
            /* style */         CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
            /* lpfnWndProc */   main_window_behavior,
            /* cbClsExtra */    0,
            /* cbWndExtra */    0,
            /* hInstance */     instance,
            /* hIcon */         0, //TODO: Add icon
            /* hCursor */       0,
            /* hbrBackground */ 0,
            /* lpszMenuName */  0,
            /* lpszClassName */ "main_window",
            /* hIconSm */       0 //TODO: Add icon
        };

        RegisterClassExA(&window.w_class);

        window.handle = CreateWindowExA(
            /* dwExStyle */    0,
            /* lpClassName */  window.w_class.lpszClassName,
            /* lpWindowName */ "Handmade Tutorial",
            /* dwStyle */      WS_TILEDWINDOW | WS_VISIBLE,
            /* X */            CW_USEDEFAULT,
            /* Y */            CW_USEDEFAULT,
            /* nWidth */       CW_USEDEFAULT,
            /* nHeight */      CW_USEDEFAULT,
            /* hWndParent */   0,
            /* hMenu */        0,
            /* hInstance */    instance,
            /* lpParam */      0
        );

        window.device_context = GetDC(window.handle);
    }

    MSG message;
    while (!quit) // Program main loop
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
            // if (message.message == WM_QUIT)
            // {
            //     quit = 1;
            // }
            // else {
            // }
        }
        show_gradient(0, 0);
        update_window(window.device_context);
    }
    return message.wParam;
}

LRESULT main_window_behavior(
        HWND window_handle,
        UINT message,
        WPARAM w_param,
        LPARAM l_param)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_QUIT:
            OutputDebugStringA("Quitting App.\n");
            quit = 1;
            break;

        case WM_CLOSE:
            OutputDebugStringA("Closing Window.\n");
            quit = 1;
            break;

        case WM_DESTROY:
            OutputDebugStringA("Destroying Window.\n");
            quit = 1;
            break;

        case WM_SIZE:
            {
                OutputDebugStringA("Rezising Window.\n");
                { // Get client size
                    RECT client_rect; GetClientRect(window_handle, &client_rect);
                    window.width = client_rect.right - client_rect.left;
                    window.height = client_rect.bottom - client_rect.top;
                }
                allocate_bitmap(window.width, window.height);
            } break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window_handle, &paint);
            update_window(device_context);
            EndPaint(window_handle, &paint);
        } break;

        default:
            result = DefWindowProcA(window_handle, message, w_param, l_param);
            break;
    }
    return result;
}

internal void allocate_bitmap(int width, int height)
{
    bitmap.width = width;
    bitmap.height = height;

    { // Bitmap Info Definiiton
        bitmap.info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap.info.bmiHeader.biWidth = bitmap.width;
        bitmap.info.bmiHeader.biHeight = -bitmap.height;
        bitmap.info.bmiHeader.biPlanes = 1;
        bitmap.info.bmiHeader.biBitCount = 8*BYTES_PER_PIXEL;
        bitmap.info.bmiHeader.biCompression = BI_RGB;
    }
    int memory_size = (bitmap.width*bitmap.height)*BYTES_PER_PIXEL;

    if (bitmap.memory)
        VirtualFree(bitmap.memory, 0, MEM_RELEASE);

    bitmap.memory = VirtualAlloc(
        0,
        memory_size,
        MEM_COMMIT,
        PAGE_READWRITE
    );
}

internal void update_window(HDC device_context)
{
    StretchDIBits(
        device_context,
        0, 0, bitmap.width, bitmap.height,
        0, 0, window.width, window.height,
        bitmap.memory,
        &bitmap.info,
        DIB_RGB_COLORS, SRCCOPY
    );
}

internal void show_gradient(int x_offset, int y_offset)
{
    int pitch = bitmap.width*BYTES_PER_PIXEL;
    uint8 *row = (uint8 *) bitmap.memory;
    for(int y = 0; y < bitmap.height; ++y)
    {
        PIXEL *p = (PIXEL *) row;
        for(int x= 0; x < bitmap.width; ++x)
        {
            p -> blue = (uint8)(x + x_offset);
            p -> green = (uint8)(y + y_offset);
            p++;
        }
        row += pitch;
    }

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
