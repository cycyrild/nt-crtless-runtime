#include <phnt_windows.h>
#include <phnt.h>

#include <imports/iterator.hh>
#include <imports/resolver.hh>
#include <reloc/reloc.hh>
#include <rt/relative_range.hh>
#include <rt/runtime_info.hh>

void Bootstrap(PPEB peb)
{
    auto rtInfo = (rt::RuntimeInfo &)rt::g_RuntimeInfo;

    auto imageBase = rtInfo.imageBase();
    auto previousBase = rtInfo.relocated_image_base;
    auto runtimeInfoData = rtInfo.data;

    if (imageBase != previousBase)
    {
        bool relocated = reloc::RelocateImage(imageBase, previousBase, rtInfo.relocations.span(), runtimeInfoData);

        if (!relocated)
            abort();

        rtInfo.relocated_image_base = imageBase;
    }

    imports::ForEachImport(
        imageBase,
        rtInfo.imports.span(),
        [ldr =
             peb->Ldr](std::string_view dllName, std::string_view functionName, uint16_t ordinal, uintptr_t *iatSlot) {
            *iatSlot = imports::ResolveImport(ldr, dllName, functionName, ordinal);
        });
}
