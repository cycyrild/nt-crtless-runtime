#include <reloc/reloc.hh>

__attribute__((section(".text$runtime"))) extern "C" void Bootstrap(void)
{
    // No relocation is needed when loaded at the preferred base.
    if (g_RuntimeInfo.imageBase() == PREFERRED_IMAGE_BASE)
        return;

    uint8_t *reloc_begin = g_RuntimeInfo.relocations.address();

    bool relocated = RelocateImage(g_RuntimeInfo.imageBase(),
                                   PREFERRED_IMAGE_BASE,
                                   reloc_begin,
                                   g_RuntimeInfo.relocations.size,
                                   g_RuntimeInfo.data);

    if (!relocated)
        return;
}
