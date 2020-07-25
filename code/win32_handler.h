#include <windows.h>
#include "my_definitions.h"

typedef struct MY_WINDOW {
    HWND handle;
    WNDCLASSEXA w_class;
    HDC device_context;
    int width;
    int height;
} MY_WINDOW;

typedef struct MY_BITMAP {
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
} MY_BITMAP;

/* Prototypes */
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT main_window_behavior(HWND, UINT, WPARAM, LPARAM);
internal void allocate_bitmap(int, int);
internal void update_window(HDC);
internal void show_gradient(int, int);