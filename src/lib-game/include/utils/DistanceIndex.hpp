#pragma once

#include <DGM/dgm.hpp>
#include <map>

class [[nodiscard]] DistanceIndex final
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

class [[nodiscard]] DistanceIndex2 final
{
public:
    DistanceIndex2(const dgm::Mesh& mesh);

public:
    [[nodiscard]] int
    getDistance(const sf::Vector2f& from, const sf::Vector2f& to) const
    {
        const auto fromTile = toTileCoord(from);
        const auto toTile = toTileCoord(to);

        if (fromTile == toTile) return 0;

        try
        {
            return index
                .at(std::min(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
                .at(std::max(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
        }
        catch (...)
        {
            return 1000;
        }
    }

private:
    [[nodiscard]] sf::Vector2u toTileCoord(unsigned meshIndex) const noexcept
    {
        return sf::Vector2u(meshIndex % tilesPerRow, meshIndex / tilesPerRow);
    }

    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept
    {
        return sf::Vector2u(worldCoord / voxelSize);
    }

private:
    const unsigned tilesPerRow;
    const float voxelSize;
    std::map<
        sf::Vector2u,
        std::map<sf::Vector2u, int, dgm::Utility::less<sf::Vector2u>>,
        dgm::Utility::less<sf::Vector2u>>
        index;
};

class [[nodiscard]] DistanceIndex3 final
{
public:
    DistanceIndex3(const dgm::Mesh& mesh);

public:
    /*[[nodiscard]] int
    getDistance(const sf::Vector2f& from, const sf::Vector2f& to) const
    {
        const auto fromTile = toTileCoord(from);
        const auto toTile = toTileCoord(to);

        if (fromTile == toTile) return 0;

        try
        {
            return index
                .at(std::min(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
                .at(std::max(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
        }
        catch (...)
        {
            return 1000;
        }
    }*/

private:
    [[nodiscard]] sf::Vector2u toTileCoord(unsigned meshIndex) const noexcept
    {
        return sf::Vector2u(meshIndex % tilesPerRow, meshIndex / tilesPerRow);
    }

    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept
    {
        return sf::Vector2u(worldCoord / voxelSize);
    }

private:
    const unsigned tilesPerRow;
    const float voxelSize;
    std::vector<std::vector<int>> index;
};

// Ifless
class [[nodiscard]] DistanceIndex4 final
{
public:
    DistanceIndex4(const dgm::Mesh& mesh);

public:
    /*[[nodiscard]] int
    getDistance(const sf::Vector2f& from, const sf::Vector2f& to) const
    {
        const auto fromTile = toTileCoord(from);
        const auto toTile = toTileCoord(to);

        if (fromTile == toTile) return 0;

        try
        {
            return index
                .at(std::min(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
                .at(std::max(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
        }
        catch (...)
        {
            return 1000;
        }
    }*/

private:
    [[nodiscard]] sf::Vector2u toTileCoord(unsigned meshIndex) const noexcept
    {
        return sf::Vector2u(meshIndex % tilesPerRow, meshIndex / tilesPerRow);
    }

    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept
    {
        return sf::Vector2u(worldCoord / voxelSize);
    }

private:
    const unsigned tilesPerRow;
    const float voxelSize;
    std::vector<std::vector<int>> index;
};

class [[nodiscard]] DistanceIndex5 final
{
public:
    DistanceIndex5(const dgm::Mesh& mesh);

public:
    /*[[nodiscard]] int
    getDistance(const sf::Vector2f& from, const sf::Vector2f& to) const
    {
        const auto fromTile = toTileCoord(from);
        const auto toTile = toTileCoord(to);

        if (fromTile == toTile) return 0;

        try
        {
            return index
                .at(std::min(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}))
                .at(std::max(
                    fromTile, toTile, dgm::Utility::less<sf::Vector2u> {}));
        }
        catch (...)
        {
            return 1000;
        }
    }*/

private:
    [[nodiscard]] sf::Vector2u toTileCoord(unsigned meshIndex) const noexcept
    {
        return sf::Vector2u(meshIndex % tilesPerRow, meshIndex / tilesPerRow);
    }

    [[nodiscard]] sf::Vector2u
    toTileCoord(const sf::Vector2f& worldCoord) const noexcept
    {
        return sf::Vector2u(worldCoord / voxelSize);
    }

private:
    const unsigned tilesPerRow;
    const float voxelSize;
    std::vector<std::vector<unsigned>> index;
};
