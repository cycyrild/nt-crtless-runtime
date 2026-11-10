#include <rt/runtime_info.hh>

namespace rt
{
    __attribute__((used, retain, section(".runtime_info"))) volatile rt::RuntimeInfo g_RuntimeInfo;
}
