#include <cstdint>
#include <imports/resolver.hh>

namespace imports
{
    constexpr unsigned MAX_FORWARD_DEPTH = 16;

    uintptr_t FindLoadedModule(PEB_LDR_DATA *ldr, std::string_view name)
    {
        bool appendDll = !ctre::search<R"(\.)">(name);
        size_t length = name.size() + (appendDll ? 4 : 0);

        if (length > MAX_PATH)
            return 0;

        WCHAR wideName[MAX_PATH];

        for (size_t i = 0; i < name.size(); ++i)
            wideName[i] = (uint8_t)name[i];

        if (appendDll)
        {
            auto suffix = L".dll";

            for (size_t i = 0; i < 4; ++i)
                wideName[name.size() + i] = suffix[i];
        }

        LIST_ENTRY *head = &ldr->InLoadOrderModuleList;

        for (LIST_ENTRY *link = head->Flink; link != head; link = link->Flink)
        {
            auto module = CONTAINING_RECORD(link, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
            auto baseName = module->BaseDllName;

            if (baseName.Length / sizeof(WCHAR) != length)
                continue;

            if (_wcsnicmp(baseName.Buffer, wideName, length) == 0)
                return (uintptr_t)module->DllBase;
        }

        return 0;
    }

    uintptr_t ResolveImportImpl(
        PEB_LDR_DATA *ldr, std::string_view dllName, std::string_view functionName, uint16_t ordinal, unsigned depth)
    {
        if (depth >= MAX_FORWARD_DEPTH)
            return 0;

        const uintptr_t base = FindLoadedModule(ldr, dllName);

        if (!base)
            return 0;

        auto dos = (const IMAGE_DOS_HEADER *)base;
        auto nt = (const IMAGE_NT_HEADERS64 *)(base + dos->e_lfanew);

        auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

        if (!directory.VirtualAddress)
            return 0;

        auto exports = (const IMAGE_EXPORT_DIRECTORY *)(base + directory.VirtualAddress);
        auto functions = (const uint32_t *)(base + exports->AddressOfFunctions);

        uint32_t index = 0;

        if (!functionName.empty())
        {
            auto names = (const uint32_t *)(base + exports->AddressOfNames);
            auto ordinals = (const uint16_t *)(base + exports->AddressOfNameOrdinals);

            bool found = false;

            for (uint32_t n = 0; n < exports->NumberOfNames; ++n)
            {
                auto name = (const char *)(base + names[n]);

                if (std::string_view(name) == functionName)
                {
                    index = ordinals[n];
                    found = true;
                    break;
                }
            }

            if (!found)
                return 0;
        }
        else
        {
            if (ordinal < exports->Base)
                return 0;

            index = ordinal - exports->Base;
        }

        if (index >= exports->NumberOfFunctions)
            return 0;

        uint32_t rva = functions[index];

        if (!rva)
            return 0;

        uint64_t exportEnd = uint64_t(directory.VirtualAddress) + directory.Size;

        if (rva < directory.VirtualAddress || uint64_t(rva) >= exportEnd)
            return base + rva;

        std::string_view forward{(const char *)(base + rva)};

        if (auto match = ctre::match<R"((.+)\.([^.]+))">(forward))
        {
            std::string_view targetDll = match.get<1>().to_view();
            std::string_view targetName = match.get<2>().to_view();

            if (auto ordinalMatch = ctre::match<"#([0-9]+)">(targetName))
            {
                uint32_t targetOrdinal = 0;

                for (char digit : ordinalMatch.get<1>().to_view())
                {
                    targetOrdinal = targetOrdinal * 10 + (digit - '0');

                    if (targetOrdinal > UINT16_MAX)
                        return 0;
                }

                return ResolveImportImpl(ldr, targetDll, {}, (uint16_t)targetOrdinal, depth + 1);
            }

            if (targetName.front() == '#')
                return 0;

            return ResolveImportImpl(ldr, targetDll, targetName, 0, depth + 1);
        }

        return 0;
    }

    uintptr_t ResolveImport(PEB_LDR_DATA *ldr,
                            std::string_view dllName,
                            std::string_view functionName,
                            uint16_t ordinal)
    {
        return ResolveImportImpl(ldr, dllName, functionName, ordinal, 0);
    }
}
