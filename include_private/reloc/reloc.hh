#pragma once

#include <rt/runtime_info.hh>

namespace reloc
{
    bool RelocateImage(uintptr_t image_base,
                       uintptr_t preferred_image_base,
                       std::span<std::byte> reloc,
                       rt::RelativeRange target_range);
}
