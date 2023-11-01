#include "render/Raycaster.hpp"

inline float mymodff(float f) noexcept
{
    float v;
    return std::modff(f, &v);
}

Raycaster::Raycaster(const sf::Vector2u& voxelSize) : RaycastingBase(voxelSize)
{
    faces.reserve(1024);
    upperFaces.reserve(1024);
    flats.reserve(1024);
    thingIds.reserve(100);
}

void Raycaster::prepare()
{
    faces.clear();
    upperFaces.clear();
    upperFacesRight.clear();
    flats.clear();
    thingIds.clear();
    visitedFaces.reset();
    visitedUpperFaces.reset();
    visitedFloors.reset();
    visitedCeils.reset();
}

void Raycaster::finalize()
{
    auto sortPredicate = [](auto& a, auto& b) -> bool
    { return a.distance > b.distance; };

    faces.insert(faces.end(), upperFaces.begin(), upperFaces.end());

    std::sort(faces.begin(), faces.end(), sortPredicate);
    std::sort(flats.begin(), flats.end(), sortPredicate);

    // deduplicate thingIds
    std::sort(thingIds.begin(), thingIds.end());
    thingIds.erase(
        std::unique(thingIds.begin(), thingIds.end()), thingIds.end());
}

std::pair<sf::Vector2f, sf::Vector2f>
Raycaster::computeVertexCoordsFromTileCoord(
    unsigned realSide, const sf::Vector2f& tile) const noexcept
{
    if (realSide == 0) // looking from north to south
    {
        return { { tile.x + 1.f, tile.y }, tile };
    }
    else if (realSide == 1) // looking from east to west
    {
        return { { tile.x + 1.f, tile.y + 1.f }, { tile.x + 1.f, tile.y } };
    }
    else if (realSide == 2) // looking from south to north
    {
        return { { tile.x, tile.y + 1.f }, { tile.x + 1.f, tile.y + 1.f } };
    }
    else if (realSide == 3) // looking from east to west
    {
        return { tile, { tile.x, tile.y + 1.f } };
    }
}

void Raycaster::addFloorFlatAndThings(
    const sf::Vector2u& tile,
    const sf::Vector2f& pos,
    unsigned tileId,
    const dgm::SpatialIndex<EntityIndexType>& spatialIndex)
{
    visitedFloors[tileId] = true;
    addFlat(tile, pos, tileId, -0.5f);

    // Add sprites, ignore those that are fully obscured
    auto&& candidates =
        spatialIndex.getOverlapCandidates(getTileBoundingBox(tile));
    thingIds.insert(thingIds.end(), candidates.begin(), candidates.end());
    // TODO: use set to deduplicate right away?
}

void Raycaster::addCeilFlat(
    const sf::Vector2u& tile,
    const sf::Vector2f& pos,
    unsigned tileId,
    bool upperPlaneTracker)
{
    visitedCeils[tileId] = true;
    addFlat(tile, pos, tileId, upperPlaneTracker ? 0.5f : 1.5f);
}

void Raycaster::addFlat(
    const sf::Vector2u& tile,
    const sf::Vector2f& pos,
    const unsigned tileId,
    const float heightHint)
{

    auto&& baseTile = sf::Vector2f(tile);
    auto&& baseTile2 = sf::Vector2f { baseTile.x + 1.f, baseTile.y + 1.f };
    flats.push_back(Flat {
        .vertices = { baseTile,
                      { baseTile.x + 1.f, baseTile.y },
                      baseTile2,
                      { baseTile.x, baseTile.y + 1.f } },
        .heightHint = heightHint,
        .distance = dgm::Math::getSize((baseTile + baseTile2) / 2.f - pos),
        .tileId = tileId });
}

float Raycaster::getTexHint(
    unsigned realSide, const sf::Vector2f& hitloc) const noexcept
{
    switch (realSide)
    {
    case 0:
        return 1 - mymodff(hitloc.x);
    case 1:
        return 1 - mymodff(hitloc.y);
    case 2:
        return mymodff(hitloc.x);
    case 3:
        return mymodff(hitloc.y);
    }

    // Never happens
    return NAN;
}
