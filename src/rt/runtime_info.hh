#pragma once

#include <rt/relative_range.hh>

struct RuntimeInfo
{
    uint32_t start_rva;

    RelativeRange relocations;

    // Application code, excluding the runtime bootstrap
    RelativeRange code;

    // Application data, excluding relocation metadata
    RelativeRange data;

    uintptr_t imageBase() const volatile
    {
        return reinterpret_cast<uintptr_t>(&start) - start_rva;
    }
};

static_assert(sizeof(RuntimeInfo) == 28);
