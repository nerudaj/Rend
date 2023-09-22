#pragma once

#include <variant>

struct DummyAudioEvent
{};

using AudioEvent = std::variant<DummyAudioEvent>;
