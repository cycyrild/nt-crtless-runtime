#include <rt/runtime_info.hh>

__attribute__((section(".runtime_info"))) volatile RuntimeInfo g_RuntimeInfo{};
