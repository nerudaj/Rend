#pragma once

#include <DGM/dgm.hpp>
#include <map>

class [[nodiscard]] DistanceIndex final
{
public:
    using DistanceType = uint16_t;

public:
    DistanceIndex(const dgm::Mesh& mesh);

public:
    [[nodiscard]] DistanceType getDistance(
        const sf::Vector2f& from, const sf::Vector2f& to) const noexcept;

private:
    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept
    {
        return sf::Vector2u(worldCoord / voxelSize);
    }

    [[nodiscard]] constexpr size_t
    toTileIndex(const sf::Vector2u& coord) const noexcept
    {
        return coord.y * tilesPerRow + coord.x;
    }

private:
    const unsigned tilesPerRow = 0;
    const float voxelSize = 0.f;
    std::vector<std::vector<DistanceType>> index = {};
};
