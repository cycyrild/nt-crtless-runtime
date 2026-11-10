#include <phnt_windows.h>
#include <phnt.h>

#include <start.hh>

void Bootstrap(PPEB peb);

extern "C" __attribute__((section(".text$runtime_start"))) NTSTATUS start(PPEB peb)
{
    Bootstrap(peb);

    return NtProcessStartup(peb);
}
