#include "utils/DistanceIndex.hpp"
#include <bitset>
#include <print>

static void computeDistances(
    const dgm::Mesh& mesh,
    std::vector<DistanceIndex::DistanceType>& lookup,
    unsigned origX,
    unsigned origY)
{
    struct [[nodiscard]] Point final
    {
        size_t position;
        unsigned value;
    };

    auto toIndex = [&mesh](unsigned x, unsigned y) -> size_t
    { return y * mesh.getDataSize().x + x; };
    const auto&& startIndex = toIndex(origX, origY);

    std::deque<Point> deque = { Point { .position = startIndex, .value = 0 } };

    while (!deque.empty())
    {
        auto item = deque.front();
        deque.pop_front();

        if (mesh[item.position] == 1 || lookup[item.position] <= item.value)
            continue;

        lookup[item.position] = item.value;

        deque.emplace_back(item.position - 1, item.value + 1);
        deque.emplace_back(item.position + 1, item.value + 1);
        deque.emplace_back(
            item.position - mesh.getDataSize().x, item.value + 1);
        deque.emplace_back(
            item.position + mesh.getDataSize().x, item.value + 1);
    }
}

DistanceIndex::DistanceIndex(const dgm::Mesh& mesh)
    : tilesPerRow(mesh.getDataSize().x)
    , voxelSize(static_cast<float>(mesh.getVoxelSize().x))
{
    const auto limit = mesh.getDataSize().x * mesh.getDataSize().y;
    index = decltype(index)(limit);

    for (unsigned y = 1; y < mesh.getDataSize().y - 1; ++y)
    {
        for (unsigned x = 1; x < mesh.getDataSize().x - 1; ++x)
        {
            const auto&& i = toTileIndex({ x, y });
            if (mesh[i] == 1) continue;

            index[i] = std::vector<DistanceType>(
                limit, std::numeric_limits<DistanceType>::max());
            computeDistances(mesh, index[i], x, y);
        }
    }
}

DistanceIndex::DistanceType DistanceIndex::getDistance(
    const sf::Vector2f& from, const sf::Vector2f& to) const noexcept
{
    const auto fromTile = toTileIndex(toTileCoord(from));
    const auto toTile = toTileIndex(toTileCoord(to));
    try
    {
        return index.at(fromTile).at(toTile);
    }
    catch (...)
    {
        return std::numeric_limits<DistanceType>::max();
    }
}
