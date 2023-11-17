#pragma once

#include "Interfaces/CommandInterface.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include "LevelMesh/DrawableLeveldMesh.hpp"
#include <SFML/System/Vector2.hpp>

class SetTileAreaInverseCommand final : public CommandInterface
{
public:
    struct TileEdit
    {
        sf::Vector2u pos;
        unsigned value;
        bool blocking;
    };

protected:
    DrawableLeveldMesh& targetMesh;
    const std::vector<TileEdit>& tilesToEdit;

public:
    SetTileAreaInverseCommand(
        DrawableLeveldMesh& targetMesh,
        const std::vector<TileEdit>& tilesToEdit)
        : targetMesh(targetMesh), tilesToEdit(tilesToEdit)
    {
    }

public:
    virtual void exec() override;
};

class SetTileAreaCommand final : public UndoableCommandInterface
{
protected:
    DrawableLeveldMesh& targetMesh;
    sf::Vector2u start, end;
    unsigned value;
    bool blocking;
    bool fill;

    std::vector<SetTileAreaInverseCommand::TileEdit> reverseEdits;

public:
    SetTileAreaCommand(
        DrawableLeveldMesh& targetMesh,
        sf::Vector2u start,
        sf::Vector2u end,
        unsigned value,
        bool blocking,
        bool fill)
        : targetMesh(targetMesh)
        , start(start)
        , end(end)
        , value(value)
        , blocking(blocking)
        , fill(fill)
    {
    }

public:
    virtual void exec() override;

    [[nodiscard]] virtual mem::Box<CommandInterface>
    getInverse() const override;
};
