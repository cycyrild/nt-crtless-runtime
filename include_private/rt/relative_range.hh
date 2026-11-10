#pragma once

#include <cstdint>
#include <span>

extern "C" void start();

namespace rt
{
    // Describes a range relative to the runtime entry point start()
    struct RelativeRange
    {
      private:
        uint32_t offset_from_start;

      public:
        uint32_t size;

        std::byte *address() const volatile
        {
            return reinterpret_cast<std::byte *>(&start) + offset_from_start;
        }

        bool contains(const void *ptr, size_t length) const volatile
        {
            uintptr_t begin = reinterpret_cast<uintptr_t>(address());
            uintptr_t value = reinterpret_cast<uintptr_t>(ptr);

            if (value < begin)
                return false;

            uintptr_t offset = value - begin;

            return offset <= size && length <= size - offset;
        }

        std::span<std::byte> span() const volatile
        {
            return {address(), size};
        }
    };

    static_assert(sizeof(RelativeRange) == 8);

}
