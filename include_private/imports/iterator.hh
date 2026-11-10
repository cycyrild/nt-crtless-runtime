#pragma once

#include <phnt_windows.h>
#include <phnt.h>

#include <cstdint>
#include <functional>
#include <span>
#include <string_view>

namespace imports
{
    using ImportCallback = void(std::string_view dllName,
                                std::string_view functionName,
                                uint16_t ordinal,
                                uintptr_t *iatSlot);

    void ForEachImport(uintptr_t imageBase,
                       std::span<std::byte> imports,
                       std::move_only_function<ImportCallback> callback);
}
