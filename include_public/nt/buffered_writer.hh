#pragma once
#include <phnt_windows.h>
#include <phnt.h>

#include <array>

namespace nt
{
    template <typename Char> struct buffered_writer
    {
        using value_type = Char;

        explicit buffered_writer(HANDLE h) noexcept;

        void flush();
        void push_back(Char c);

        HANDLE handle;
        std::array<Char, 128> data;
        std::size_t used = 0;
        bool ok = true;
    };

    extern template struct buffered_writer<char>;
    extern template struct buffered_writer<wchar_t>;
}
