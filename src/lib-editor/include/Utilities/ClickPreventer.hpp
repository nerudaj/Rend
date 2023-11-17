#pragma once

#include <DGM/dgm.hpp>

class ClickPreventer
{
public:
    void preventClickForNextNFrames(const unsigned frameCount)
    {
        framesLeft = frameCount;
    }

    void update()
    {
        if (framesLeft > 0) --framesLeft;
    }

    bool shouldPreventEvents() const noexcept
    {
        return framesLeft > 0;
    }

private:
    unsigned framesLeft = 0;
};