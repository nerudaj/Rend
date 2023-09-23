#pragma once

#include <DGM/dgm.hpp>
#include <core/Enums.hpp>
#include <variant>

struct PickablePickedUpGameEvent
{
    unsigned id;

    PickablePickedUpGameEvent(unsigned id) : id(id) {}
};

using GameEvent = std::variant<PickablePickedUpGameEvent>;
