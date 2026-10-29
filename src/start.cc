#include <reloc/reloc.hh>


__attribute__((section(".text$runtime"))) extern "C" void Bootstrap(void)
{
    constexpr uintptr_t preferred_image_base = 0x180000000ULL;

    uintptr_t runtime_image_base = reinterpret_cast<uintptr_t>(&start) - g_RuntimeInfo.start_rva;

    // No relocation is needed when loaded at the preferred base.
    if (runtime_image_base == preferred_image_base)
        return;

    uint8_t *reloc_begin = g_RuntimeInfo.relocations.address();

    bool relocated = RelocateImage(
        runtime_image_base, preferred_image_base, reloc_begin, g_RuntimeInfo.relocations.size, g_RuntimeInfo.data);

    if (!relocated)
        return;
}
