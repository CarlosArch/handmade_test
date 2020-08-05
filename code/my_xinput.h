#include <Xinput.h>
#include <libloaderapi.h>
#include "my_definitions.h"

/* All of this is to not crash if there's no Xinput on the machine,
   but it "should" work as normal xinput */
#define XINPUTGETSTATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define XINPUTSETSTATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)

typedef XINPUTGETSTATE(xinputgetfunc);
typedef XINPUTSETSTATE(xinputsetfunc);

XINPUTGETSTATE(XInputGetStateStub) {return ERROR_DEVICE_NOT_CONNECTED;}
XINPUTSETSTATE(XInputSetStateStub) {return ERROR_DEVICE_NOT_CONNECTED;}

global xinputgetfunc* XInputGetState_ = XInputGetStateStub;
global xinputsetfunc* XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

void load_xinput(void)
{
    HMODULE xinput_library = 0;
    if (!xinput_library) xinput_library = LoadLibraryA("xinput1_4.dll");
    if (!xinput_library) xinput_library = LoadLibraryA("xinput1_3.dll");

    if(xinput_library)
    {
        XInputGetState_ = (xinputgetfunc*) GetProcAddress(xinput_library, "XInputGetState");
        XInputSetState_ = (xinputsetfunc*) GetProcAddress(xinput_library, "XInputSetState");
    }

}