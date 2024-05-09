#pragma once

#include "Interfaces/UndoableCommandInterface.hpp"
#include "LevelMesh/DrawableLeveldMesh.hpp"
#include "Utilities/InitGuard.hpp"
#include <Memory.hpp>

class [[nodiscard]] SetTilePropertyCommand final
    : public UndoableCommandInterface
{
public:
    SetTilePropertyCommand(
        DrawableLeveldMesh& map,
        const sf::Vector2u& tileCoord,
        unsigned tileValue,
        bool solid)
        : map(map), tileCoord(tileCoord), tileValue(tileValue), solid(solid)
    {
    }

public:
    virtual void exec() override;

    virtual mem::Box<CommandInterface> getInverse() const override;

protected:
    DrawableLeveldMesh& map;
    sf::Vector2u tileCoord;
    unsigned tileValue;
    bool solid;
    InitGuard<unsigned> oldTileValue;
    InitGuard<bool> oldSolid;
};