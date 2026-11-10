#include <phnt_windows.h>
#include <phnt.h>

#include <start.hh>

void Bootstrap(PPEB peb);

extern "C" __attribute__((section(".text$runtime_start"))) BOOLEAN start(HINSTANCE instance,
                                                                         DWORD reason,
                                                                         PVOID reserved)
{
    PPEB peb = NtCurrentPeb();

    if (reason == DLL_PROCESS_ATTACH)
        Bootstrap(peb);

    return DllMain(instance, reason, reserved);
}
