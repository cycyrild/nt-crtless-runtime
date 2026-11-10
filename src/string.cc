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

extern "C" int _wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n)
{
    while (n--)
    {
        wint_t c1 = towlower((wint_t)*s1++);
        wint_t c2 = towlower((wint_t)*s2++);

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;

        if (c1 == L'\0')
            return 0;
    }

    return 0;
}

extern "C" size_t strlen(const char *str)
{
    const char *s = str;

    while (*s)
        ++s;

    return (size_t)(s - str);
}
