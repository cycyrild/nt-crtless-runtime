extern "C" __declspec(noreturn) void __cdecl abort()
{
    __builtin_trap();
}
