#include <windows.h>

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd)
{
    MessageBox(
        0, "Mensaje", "Titulo",
        MB_OK|MB_ICONINFORMATION);
}