#include <nt/buffered_writer.hh>

namespace nt
{
    template <typename Char> buffered_writer<Char>::buffered_writer(HANDLE h) noexcept : handle(h)
    {
    }

    template <typename Char> void buffered_writer<Char>::flush()
    {
        if (!ok || !used)
            return;

        IO_STATUS_BLOCK iosb;

        ULONG size = static_cast<ULONG>(used * sizeof(Char));

        NTSTATUS status = NtWriteFile(handle, nullptr, nullptr, nullptr, &iosb, data.data(), size, nullptr, nullptr);

        ok = status == STATUS_SUCCESS && iosb.Information == size;

        if (ok)
            used = 0;
    }

    template <typename Char> void buffered_writer<Char>::push_back(Char c)
    {
        if (!ok)
            return;

        data[used++] = c;

        if (used == data.size())
            flush();
    }

    template struct buffered_writer<char>;
    template struct buffered_writer<wchar_t>;
}
