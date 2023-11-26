#pragma once

#include <variant>

struct EventRenderToggle
{
    bool fpsDisplay = false;
    bool topDownRender = false;
    bool ditheredShader = false;
};

using RenderingEvent = std::variant<EventRenderToggle>;
