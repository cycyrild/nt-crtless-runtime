#include <cstddef>
#include <cwchar>
#include <cwctype>

extern "C" size_t wcslen(const wchar_t *str)
{
    const wchar_t *s = str;

    while (*s)
        ++s;

    return (s - str);
}

extern "C" wint_t towlower(wint_t wc)
{
    if (wc == WEOF)
        return wc;

    if (wc >= L'A' && wc <= L'Z')
        return wc - L'A' + L'a';

    return wc;
}
