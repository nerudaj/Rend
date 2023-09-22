#pragma once

#include <variant>

struct EventRenderToggle
{
    bool fpsDisplay = false;
    bool topDownRender = false;
};

using RenderingEvent = std::variant<EventRenderToggle>;
