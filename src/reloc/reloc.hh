#pragma once

#include <rt/runtime_info.hh>

extern volatile RuntimeInfo g_RuntimeInfo;

bool RelocateImage(uintptr_t image_base,
                   uintptr_t preferred_image_base,
                   uint8_t *reloc_begin,
                   uint32_t reloc_size,
                   const volatile RelativeRange &target_range);
