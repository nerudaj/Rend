#pragma once

class Sizers
{
public:
    static unsigned GetSystemDPI();

    static unsigned GetMenuBarHeight();

    static unsigned GetMenuBarTextHeight();

    static [[nodiscard]] unsigned getBaseFontSize()
    {
        return GetMenuBarTextHeight();
    }

    static [[nodiscard]] unsigned getBaseRowHeight()
    {
        return GetMenuBarHeight();
    }
};
