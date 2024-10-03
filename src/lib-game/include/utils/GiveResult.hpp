#pragma once

struct [[nodiscard]] GiveResult final
{
    bool given = true;
    bool removePickup = true;
    bool playSound = true;
};
