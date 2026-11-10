#include <phnt_windows.h>
#include <phnt.h>

#include <rt/runtime_info.hh>

namespace reloc
{
    bool ProcessBaseRelocations(
        uintptr_t image_base, uintptr_t delta, std::span<std::byte> reloc, rt::RelativeRange target_range, bool apply)
    {
        while (!reloc.empty())
        {
            if (reloc.size() < sizeof(IMAGE_BASE_RELOCATION))
                return false;

            auto *block = (const IMAGE_BASE_RELOCATION *)reloc.data();

            if (block->SizeOfBlock < sizeof(IMAGE_BASE_RELOCATION))
                return false;

            if (block->SizeOfBlock > reloc.size())
                return false;

            uint32_t records_size = block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION);

            if (records_size % sizeof(IMAGE_RELOCATION_RECORD) != 0)
                return false;

            uint32_t record_count = records_size / sizeof(IMAGE_RELOCATION_RECORD);

            auto *records = (const IMAGE_RELOCATION_RECORD *)(reloc.data() + sizeof(IMAGE_BASE_RELOCATION));

            for (uint32_t i = 0; i < record_count; ++i)
            {
                const IMAGE_RELOCATION_RECORD &record = records[i];

                if (record.Type == IMAGE_REL_BASED_ABSOLUTE)
                    continue;

                if (record.Type != IMAGE_REL_BASED_DIR64)
                    return false;

                const uintptr_t rva = (uintptr_t)block->VirtualAddress + record.Offset;

                if (rva > UINTPTR_MAX - image_base)
                    return false;

                auto *target = (uint64_t *)(image_base + rva);

                if (!target_range.contains(target, sizeof(*target)))
                    return false;

                if (apply)
                    *target += (uint64_t)delta;
            }

            reloc = reloc.subspan(block->SizeOfBlock);
        }

        return true;
    }

    bool RelocateImage(uintptr_t image_base,
                       uintptr_t previous_base,
                       std::span<std::byte> reloc,
                       rt::RelativeRange target_range)
    {
        if (image_base == previous_base)
            return true;

        const uintptr_t delta = image_base - previous_base;

        if (!ProcessBaseRelocations(image_base, delta, reloc, target_range, false))
            return false;

        return ProcessBaseRelocations(image_base, delta, reloc, target_range, true);
    }
}
