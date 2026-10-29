#include <phnt_windows.h>
#include <phnt.h>
#include <rt/runtime_info.hh>

__attribute__((section(".text$runtime_reloc"))) static bool ApplyBaseRelocations(
    uintptr_t image_base,
    uintptr_t delta,
    uint8_t *reloc_begin,
    uint32_t reloc_size,
    const volatile RelativeRange &target_range)
{
    uint8_t *cursor = reloc_begin;
    uint8_t *end = reloc_begin + reloc_size;

    while (cursor < end)
    {
        if (static_cast<uintptr_t>(end - cursor) < sizeof(IMAGE_BASE_RELOCATION))
            return false;

        IMAGE_BASE_RELOCATION *block = reinterpret_cast<IMAGE_BASE_RELOCATION *>(cursor);

        if (block->SizeOfBlock < sizeof(IMAGE_BASE_RELOCATION))
            return false;

        if (block->SizeOfBlock > static_cast<uintptr_t>(end - cursor))
            return false;

        uint32_t records_size = block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION);

        if ((records_size % sizeof(IMAGE_RELOCATION_RECORD)) != 0)
            return false;

        uint32_t record_count = records_size / sizeof(IMAGE_RELOCATION_RECORD);

        IMAGE_RELOCATION_RECORD *records =
            reinterpret_cast<IMAGE_RELOCATION_RECORD *>(cursor + sizeof(IMAGE_BASE_RELOCATION));

        for (uint32_t i = 0; i < record_count; ++i)
        {
            IMAGE_RELOCATION_RECORD &record = records[i];

            if (record.Type == IMAGE_REL_BASED_ABSOLUTE)
                continue;

            if (record.Type != IMAGE_REL_BASED_DIR64)
                return false;

            uint64_t *target = reinterpret_cast<uint64_t *>(image_base + block->VirtualAddress + record.Offset);

            if (!target_range.contains(target, sizeof(*target)))
                continue;

            *target += static_cast<uint64_t>(delta);
        }

        cursor += block->SizeOfBlock;
    }

    return true;
}

__attribute__((section(".text$runtime_reloc"))) bool RelocateImage(uintptr_t image_base,
                                                                   uintptr_t preferred_image_base,
                                                                   uint8_t *reloc_begin,
                                                                   uint32_t reloc_size,
                                                                   const volatile RelativeRange &target_range)
{
    if (image_base == preferred_image_base)
        return true;

    uintptr_t delta = image_base - preferred_image_base;

    return ApplyBaseRelocations(image_base, delta, reloc_begin, reloc_size, target_range);
}
