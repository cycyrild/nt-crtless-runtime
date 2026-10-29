#pragma once

#include <cstdint>

extern "C" void start();

// Describes a range relative to the runtime entry point start()
struct RelativeRange
{
  private:
    uint32_t offset_from_start;

  public:
    uint32_t size;

    uint8_t *address() const volatile
    {
        return reinterpret_cast<uint8_t *>(&start) + offset_from_start;
    }

    bool contains(const void *ptr, size_t length = 1) const volatile
    {
        uintptr_t begin = reinterpret_cast<uintptr_t>(address());
        uintptr_t value = reinterpret_cast<uintptr_t>(ptr);

        if (value < begin)
            return false;

        uintptr_t offset = value - begin;

        return offset <= size && length <= size - offset;
    }
};


static_assert(sizeof(RelativeRange) == 8);
