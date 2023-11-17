#pragma once

#include <cstddef>

enum class EditorState : std::size_t
{
    Mesh = 0,
    Item,
    Trigger
};
