#include <windows.h>
#include "my_xinput.h"
#include "my_dsound.h"

#include "my_definitions.h"
#include "win32_handler.h"

global uint8 quit;
global MY_WINDOW window;
global MY_BITMAP bitmap;
global LPDIRECTSOUNDBUFFER sound_buffer;
global uint8 up, down, left, right;

int WinMain(
        HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR     command_line,
        int       showcode)
{
    { // Load controller support if possible
        load_xinput();
    }

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

        allocate_bitmap(240, 180);

        sound_buffer = initialize_directsound(
            /* window_handle */      window.handle,
            /* number_of_channels */ NUMBER_OF_CHANNELS,
            /* samples_per_second */ SAMPLES_PER_SECOND,
            /* bits_per_sample */    BITS_PER_SAMPLE,
            /* buffer_size */        AUDIO_BUFFER_SIZE
        );
        IDirectSoundBuffer_Play(sound_buffer, 0, 0, DSBPLAY_LOOPING);
    }
    MSG message;
    while (!quit) // Program main loop
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        for (int index = 0; index < XUSER_MAX_COUNT; index++)
        {
            XINPUT_STATE state = {0};
            if (XInputGetState(index, &state) == NO_ERROR)
            {
                XINPUT_GAMEPAD *pad = &state.Gamepad;
                up    = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP)    == 1;
                down  = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)  == 1;
                left  = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)  == 1;
                right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == 1;
            }
        }
        { // This is very bad code.
            //play_square_wave(500);
            play_square_wave(250);

            int16 x, y;
            x -= left; x += right;
            y -= up;   y += down;
            if (y > (180-4)*8) y = (180-4)*8;
            if (y < 0) y = 0;
            if (x > (240-4)*8) x = (240-4)*8;
            if (x < 0) x = 0;
            show_blob(x/8, y/8);
            update_window(window.device_context);
        }
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
        case WM_QUIT: case WM_CLOSE: case WM_DESTROY:
        {
            OutputDebugStringA("Exiting Application.\n");
            quit = 1;
        } break;
        case WM_SYSKEYDOWN: case WM_KEYDOWN:
        case WM_SYSKEYUP:   case WM_KEYUP:
        {
            uint8 wasdown = (l_param >> 30 & 1) == 1;
            uint8 isdown =  (l_param >> 31 & 1) == 0;
            if (isdown != wasdown)
            {
                switch (w_param)
                {
                    case VK_UP:
                        if (isdown) up = 1;
                        if (wasdown) up = 0;
                        break;
                    case VK_DOWN:
                        if (isdown) down = 1;
                        if (wasdown) down = 0;
                        break;
                    case VK_LEFT:
                        if (isdown) left = 1;
                        if (wasdown) left = 0;
                        break;
                    case VK_RIGHT:
                        if (isdown) right = 1;
                        if (wasdown) right = 0;
                        break;
                }
            }
        } break;
        case WM_SIZE:
        {
            RECT client_rect; GetClientRect(window_handle, &client_rect);
            window.width = client_rect.right - client_rect.left;
            window.height = client_rect.bottom - client_rect.top;
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window_handle, &paint);
            update_window(device_context);
            EndPaint(window_handle, &paint);
        } break;
        default:
        {
            result = DefWindowProcA(window_handle, message, w_param, l_param);
        } break;
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
        bitmap.info.bmiHeader.biBitCount = 8 * BYTES_PER_PIXEL;
        bitmap.info.bmiHeader.biCompression = BI_RGB;
    }
    int memory_size = (bitmap.width*bitmap.height)*BYTES_PER_PIXEL;

    if (bitmap.memory) VirtualFree(bitmap.memory, 0, MEM_RELEASE);
    bitmap.memory = VirtualAlloc(
        0,
        memory_size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
}

internal void update_window(HDC device_context)
{
    int final_width, final_height, pad_left, pad_top;
    { // Calculate stretch ratio to keep original aspect ratio
        float width_ratio = (float)window.width / (float)bitmap.width;
        float height_ratio = (float)window.height / (float)bitmap.height;
        float ratio = min(width_ratio, height_ratio);

        final_width = bitmap.width*ratio;
        final_height = bitmap.height*ratio;

        pad_left = (window.width - final_width) / 2;
        pad_top = (window.height - final_height) / 2;
    }

    StretchDIBits(
        device_context,
        pad_left, pad_top, final_width, final_height,
        0, 0, bitmap.width, bitmap.height,
        bitmap.memory, &bitmap.info,
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
        for(int x = 0; x < bitmap.width; ++x)
        {
            p -> blue = (uint8)(x + x_offset)*8;
            p -> green = (uint8)(y + y_offset)*8;
            p++;
        }
        row += pitch;
    }
}

internal void show_blob(int x_pos, int y_pos)
{
    int pitch = bitmap.width*BYTES_PER_PIXEL;
    uint8 *row = (uint8 *) bitmap.memory;
    for(int y = 0; y < bitmap.height; ++y)
    {
        PIXEL *p = (PIXEL *) row;
        for(int x = 0; x < bitmap.width; ++x)
        {
            uint8 show = (x >= x_pos && x < x_pos+4 && y >= y_pos && y < y_pos+4);
            p -> blue =  (uint8)(show ? 0xFF : 0);
            p -> green = (uint8)(show ? 0xFF : 0);
            p -> red =   (uint8)(show ? 0xFF : 0);
            p++;
        }
        row += pitch;
    }
}

internal void play_square_wave(int tone)
{
    persistent uint64 index = 0;
    index += 1;
    if (index >= AUDIO_BUFFER_SIZE) index = 0;

    DWORD play_cursor;
    DWORD write_cursor;
    IDirectSoundBuffer_GetCurrentPosition(
        sound_buffer,
        &play_cursor,
        &write_cursor
    );

    void* audio_block_1;
    DWORD audio_bytes_1;
    void* audio_block_2;
    DWORD audio_bytes_2;
    
    IDirectSoundBuffer_Lock(
        /* IDirectSound */  sound_buffer,
        /* dwWriteCursor */ index,
        /* dwWriteBytes */  AUDIO_BUFFER_SIZE/2,
        /* AudioPtr1 */     &audio_block_1, &audio_bytes_1,
        /* AudioPtr2 */     &audio_block_2, &audio_bytes_2,
        /* dwFlags */       0
    );

    int16* block1_pointer = (int16*) audio_block_1;
    for (uint16 sample_index = 0;
         sample_index < audio_bytes_1/NUMBER_OF_CHANNELS;
         sample_index++)
    {
        int16 value = (index++/(SAMPLES_PER_SECOND/tone)) % 2 ? 500 : -500;
        *block1_pointer++ = value;
        *block1_pointer++ = value;
    }
    int16* block2_pointer = (int16*) audio_block_2;
    for (uint16 sample_index = 0;
         sample_index < audio_bytes_2/NUMBER_OF_CHANNELS;
         sample_index++)
    {
        int16 value = (index++/(SAMPLES_PER_SECOND/tone)) % 2 ? 500 : -500;
        *block2_pointer++ = value;
        *block2_pointer++ = value;
    }
    IDirectSoundBuffer_Unlock(
        sound_buffer,
        &audio_block_1, audio_bytes_1,
        &audio_block_2, audio_bytes_2
    );
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
