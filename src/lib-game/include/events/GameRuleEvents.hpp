#pragma once

#include <variant>

struct DummyGameEvent
{};

using GameEvent = std::variant<DummyGameEvent>;
