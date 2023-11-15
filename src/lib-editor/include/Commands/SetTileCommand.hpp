#pragma once

#include "Interfaces/CommandInterface.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include "LevelMesh/DrawableLeveldMesh.hpp"
#include "Utilities/InitGuard.hpp"
#include <SFML/System/Vector2.hpp>

import Memory;

class SetTileCommand final : public UndoableCommandInterface
{
protected:
    DrawableLeveldMesh& targetMesh;
    sf::Vector2u tilePos;
    unsigned value = 0;
    bool blocking = false;
    InitGuard<unsigned> oldValue;
    InitGuard<bool> oldBlocking;

public:
    SetTileCommand(
        DrawableLeveldMesh& targetMesh,
        const sf::Vector2u& tilePos,
        unsigned value,
        bool blocking)
        : targetMesh(targetMesh)
        , tilePos(tilePos)
        , value(value)
        , blocking(blocking)
    {
    }

public:
    void exec() override;

    [[nodiscard]] mem::Box<CommandInterface> getInverse() const override;
};
