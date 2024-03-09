#pragma once

class Sizers
{
public:
    static unsigned GetSystemDPI();

    static unsigned GetMenuBarHeight();

    static unsigned GetMenuBarTextHeight();

    static [[nodiscard]] unsigned getBaseTextSize()
    {
        return GetMenuBarTextHeight();
    }

    static [[nodiscard]] unsigned getBaseContainerHeight()
    {
        return GetMenuBarHeight();
    }
};
