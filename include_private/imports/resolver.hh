#pragma once

#include <phnt_windows.h>
#include <phnt.h>

#include <cstdint>
#include <ctre.hpp>
#include <string_view>

namespace imports
{
    uintptr_t ResolveImport(PEB_LDR_DATA *ldr,
                            std::string_view dllName,
                            std::string_view functionName,
                            uint16_t ordinal);
}
