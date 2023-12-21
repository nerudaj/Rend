#pragma once

#include "LevelMesh/DrawableDgmMesh.hpp"
#include "LevelMesh/DrawableGrid.hpp"
#include "LevelMesh/PollableTileMap.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>

// Representation of Leveld mesh that can be drawn
class DrawableLeveldMesh final
{
protected:
    DrawableGrid grid;
    DrawableDgmMesh mesh;
    PollableTileMap map;
    sf::RectangleShape darkOverlay;

public:
    DrawableLeveldMesh() = default;

    DrawableLeveldMesh(sf::Texture& texture, dgm::Clip clip);

    DrawableLeveldMesh(DrawableLeveldMesh&& other) noexcept : map(other.map) {}

    DrawableLeveldMesh(const DrawableLeveldMesh&) = delete;

    DrawableLeveldMesh& operator=(DrawableLeveldMesh&& other) noexcept
    {
        map = other.map;
        return *this;
    }

public:
    void drawTo(
        tgui::CanvasSFML::Ptr canvas,
        bool renderCollisionMesh,
        bool isTopLevel);

    void build(
        const std::vector<int>& tiles,
        const std::vector<int>& blocks,
        const sf::Vector2u& dataSize);

    void setTileValue(const sf::Vector2u& tilePos, unsigned value)
    {
        map.changeTile(tilePos.x, tilePos.y, value);
    }

    void setTileSolid(const sf::Vector2u& tilePos, bool solid)
    {
        mesh.setTileSolid(tilePos.x, tilePos.y, solid);
    }

    [[nodiscard]] const sf::Vector2u& getMapDimensions() const
    {
        return mesh.getOriginalMesh().getDataSize();
    }

    [[nodiscard]] const sf::Vector2u& getTileSize() const noexcept
    {
        return map.getClip().getFrameSize();
    }

    [[nodiscard]] unsigned getTileValue(const sf::Vector2u& tilePos) const
    {
        return map.getTileValue(tilePos.x, tilePos.y);
    }

    [[nodiscard]] unsigned isTileSolid(const sf::Vector2u& tilePos) const
    {
        return mesh.isTileSolid(tilePos.x, tilePos.y);
    }
};