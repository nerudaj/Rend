#include "Configs/Sizers.hpp"
#include <WinUser.h>
#include <Windows.h>

unsigned Sizers::GetSystemDPI()
{
    return GetDpiForSystem();
}

unsigned Sizers::GetMenuBarHeight()
{
    const unsigned dpi = GetSystemDPI();
    return GetSystemMetricsForDpi(SM_CYCAPTION, dpi)
           + GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi)
           + GetSystemMetricsForDpi(SM_CYEDGE, dpi) * 2;
}

unsigned Sizers::GetMenuBarTextHeight()
{
    return static_cast<unsigned>(GetMenuBarHeight() / 2.55f);
}
