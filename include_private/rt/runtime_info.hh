#pragma once

#include <rt/relative_range.hh>

namespace rt
{
    struct RuntimeInfo
    {
        uint64_t start_rva;

        uintptr_t relocated_image_base;

        RelativeRange relocations;
        RelativeRange imports;

        // Application code, excluding the runtime bootstrap
        RelativeRange code;

        // Application data, excluding relocation metadata
        RelativeRange data;

        uintptr_t imageBase() const volatile
        {
            return reinterpret_cast<uintptr_t>(&start) - start_rva;
        }
    };

    static_assert(sizeof(RuntimeInfo) == 48);

    extern volatile rt::RuntimeInfo g_RuntimeInfo;
}
