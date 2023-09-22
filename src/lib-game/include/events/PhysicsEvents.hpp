#pragma once

#include <variant>

struct DummyPhysicsEvent
{};

using PhysicsEvent = std::variant<DummyPhysicsEvent>;
