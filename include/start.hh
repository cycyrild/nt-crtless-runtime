#pragma once

#include <windows.h>
#include <winternl.h>

// Custom NtProcessStartup
extern "C" NTSTATUS NtProcessStartup(PPEB peb, void** initialStack);

// Custom DllMain
extern "C" BOOLEAN NTAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved, void** initialStack);
