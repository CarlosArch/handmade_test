#include <windows.h>
#include <stdint.h>

#define internal static
#define persistent static
#define global static

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global uint8 quit;
global BITMAPINFO bitmap_info;
global void *bitmap_memory;
global int bitmap_w;
global int bitmap_h;
global int bytes_per_pixel;

typedef struct {
    uint8 blue;
    uint8 green;
    uint8 red;
    uint8 padding;
} single_pixel;

/* Prototypes */
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT main_window_behavior(HWND, UINT, WPARAM, LPARAM);
internal void allocate_bitmap(int, int);
internal void update_window(HDC, RECT*);
internal void show_gradient(int, int);

/* Functions */
int WinMain(
        HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR command_line,
        int showcode)
{
    HWND window_handle;
    { // Window Creation
        WNDCLASSEX main_window_class; { // Definition
            main_window_class.cbSize = sizeof(WNDCLASSEX);
            main_window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            main_window_class.lpfnWndProc = main_window_behavior;
            main_window_class.hInstance = instance;
            main_window_class.lpszClassName = "main_window";
            main_window_class.hIcon = 0; // TODO: Add icons
            main_window_class.hIconSm = 0; // TODO: Add icons

            main_window_class.cbClsExtra = 0;
            main_window_class.cbWndExtra = 0;
            main_window_class.hCursor = 0;
            main_window_class.hbrBackground = 0;
            main_window_class.lpszMenuName = 0;
        }

        RegisterClassExA(&main_window_class);

        window_handle = CreateWindowExA(
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
        int x = 0;
        int y = 0;
        while (!quit)
        {
            while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                    return 0;
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }
            show_gradient(x, y);
            x++; y++;
            HDC device_context = GetDC(window_handle);
            RECT client_rect; GetClientRect(window_handle, &client_rect);
            update_window(device_context, &client_rect);
            ReleaseDC(window_handle, device_context);
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
            quit = 1;
            break;

        case WM_DESTROY:
            OutputDebugStringA("Destroying Window.\n");
            quit = 1;
            break;

        case WM_SIZE:
            {
                OutputDebugStringA("Rezising Window.\n");
                int w, h; { // Get client size
                    RECT client_rect; GetClientRect(window, &client_rect);
                    w = client_rect.right - client_rect.left;
                    h = client_rect.bottom - client_rect.top;
                }
                allocate_bitmap(w, h);
            } break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);
            { // Paint process.
                int x = paint.rcPaint.left;
                int y = paint.rcPaint.top;
                int w = paint.rcPaint.right - paint.rcPaint.left;
                int h = paint.rcPaint.bottom - paint.rcPaint.top;
                RECT client_rect; GetClientRect(window, &client_rect);
                update_window(device_context, &client_rect);
            }
            EndPaint(window, &paint);
        } break;

        default:
            result = DefWindowProcA(window, message, w_param, l_param);
            break;
    }
    return result;
}

internal void allocate_bitmap(int width, int height)
{
    bitmap_w = width;
    bitmap_h = height;

    { // Bitmap Info Definiiton
        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = bitmap_w;
        bitmap_info.bmiHeader.biHeight = -bitmap_h;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
    }
    bytes_per_pixel = bitmap_info.bmiHeader.biBitCount / 8;
    int bitmap_memory_size = (bitmap_w*bitmap_h)*bytes_per_pixel;

    if (bitmap_memory)
        VirtualFree(bitmap_memory, 0, MEM_RELEASE);

    bitmap_memory = VirtualAlloc(
        0,
        bitmap_memory_size,
        MEM_COMMIT,
        PAGE_READWRITE
    );
}

internal void update_window(
    HDC device_context,
    RECT *window_rect)
{
    int window_w = window_rect->right - window_rect->left;
    int window_h = window_rect->bottom - window_rect->top;
    StretchDIBits(
        device_context,
        0, 0, bitmap_w, bitmap_h,
        0, 0, window_w, window_h,
        bitmap_memory,
        &bitmap_info,
        DIB_RGB_COLORS, SRCCOPY
    );
}

internal void show_gradient(int x_offset, int y_offset)
{
    int pitch = bitmap_w*4;
    uint8 *row = (uint8 *) bitmap_memory;
    for(int y = 0; y < bitmap_h; ++y)
    {
        single_pixel *pixel = (single_pixel *) row;
        for(int x= 0; x < bitmap_w; ++x)
        {
            single_pixel pix;
            pix.blue = (uint8)(x + x_offset);
            pix.green = (uint8)(y + y_offset);

            *pixel++ = pix;
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

// MessageBox(
//     0, "Mensaje", "Titulo",
//     MB_OK|MB_ICONINFORMATION);