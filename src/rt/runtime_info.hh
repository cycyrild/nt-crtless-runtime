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
};

static_assert(sizeof(RuntimeInfo) == 28);
