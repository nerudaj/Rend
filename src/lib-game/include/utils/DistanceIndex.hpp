#pragma once

#include <DGM/dgm.hpp>
#include <map>

class [[nodiscard]] DistanceIndex
{
public:
    DistanceIndex(const dgm::Mesh& mesh);

public:
    [[nodiscard]] int
    getDistance(const sf::Vector2f& from, const sf::Vector2f& to) const;

private:
    [[nodiscard]] sf::Vector2u toTileCoord(unsigned meshIndex) const noexcept;

    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept;

private:
    const unsigned tilesPerRow;
    const float voxelSize;
    std::map<
        sf::Vector2u,
        std::map<sf::Vector2u, int, dgm::Utility::less<sf::Vector2u>>,
        dgm::Utility::less<sf::Vector2u>>
        index;
};
