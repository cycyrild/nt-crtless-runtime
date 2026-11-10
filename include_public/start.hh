#pragma once

#include <phnt_windows.h>
#include <phnt.h>

extern "C" NTSTATUS NtProcessStartup(PPEB peb);

extern "C" BOOLEAN NTAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved);
