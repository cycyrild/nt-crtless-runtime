#include <imports/iterator.hh>

namespace imports
{
    void ForEachImport(uintptr_t imageBase,
                       std::span<std::byte> imports,
                       std::move_only_function<ImportCallback> callback)
    {
        auto descriptor = (const IMAGE_IMPORT_DESCRIPTOR *)imports.data();

        for (; descriptor->Name; ++descriptor)
        {
            std::string_view dllName((const char *)(imageBase + descriptor->Name));

            auto lookupRVA = descriptor->OriginalFirstThunk ? descriptor->OriginalFirstThunk : descriptor->FirstThunk;

            auto lookup = (const IMAGE_THUNK_DATA64 *)(imageBase + lookupRVA);
            auto iat = (IMAGE_THUNK_DATA64 *)(imageBase + descriptor->FirstThunk);

            for (; lookup->u1.AddressOfData; ++lookup, ++iat)
            {
                uint64_t value = lookup->u1.AddressOfData;
                bool byOrdinal = IMAGE_SNAP_BY_ORDINAL64(value);

                std::string_view functionName;
                uint16_t ordinal = 0;

                if (byOrdinal)
                {
                    ordinal = IMAGE_ORDINAL64(value);
                }
                else
                {
                    auto name = (const IMAGE_IMPORT_BY_NAME *)(imageBase + value);
                    functionName = (const char *)name->Name;
                }

                callback(dllName, functionName, ordinal, (uintptr_t *)&iat->u1.Function);
            }
        }
    }
}
