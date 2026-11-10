#pragma once

#include <fmt/compile.h>
#include <iterator>
#include <nt/buffered_writer.hh>

#define FMT_PRINT(format, ...) ::nt::print(FMT_COMPILE(format) __VA_OPT__(, ) __VA_ARGS__)

namespace nt
{
    template <typename Format, typename... Args>
    bool __attribute__((noinline)) print(const Format &format, const Args &...args)
    {
        using Char = typename Format::char_type;

        buffered_writer<Char> buffer{NtCurrentPeb()->ProcessParameters->StandardOutput};

        fmt::format_to(std::back_inserter(buffer), format, args...);

        buffer.flush();
        return buffer.ok;
    }
}
