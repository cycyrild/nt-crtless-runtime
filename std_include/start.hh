#pragma once

#include <phnt_windows.h>
#include <phnt.h>

// Custom NtProcessStartup
extern "C" NTSTATUS NtProcessStartup(PPEB peb, void **initialStack);

// Custom DllMain
extern "C" BOOLEAN NTAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved, void **initialStack);
