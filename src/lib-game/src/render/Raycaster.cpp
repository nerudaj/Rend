#include "render/Raycaster.hpp"

inline float mymodff(float f) noexcept
{
    float v;
    return std::modff(f, &v);
}

Raycaster::Raycaster()
{
    faces.reserve(1024);
    upperFaces.reserve(1024);
    flats.reserve(1024);
    thingIds.reserve(100);
}

float Raycaster::castRay(
    const sf::Vector2f& pos,
    const sf::Vector2f& dir,
    const Level& map,
    const dgm::SpatialBuffer<GameObject>& things,
    bool leftmostRay,
    bool rightmostRay)
{
    auto getInterceptSize = [](bool side,
                               const sf::Vector2f& intercept,
                               const sf::Vector2f& rayStep) -> float
    { return side == 0 ? intercept.x - rayStep.x : intercept.y - rayStep.y; };

    auto tile = sf::Vector2u(pos);
    auto tileStep = sf::Vector2i(0, 0);
    auto rayStep = sf::Vector2f(
        std::sqrt(1 + (dir.y * dir.y) / (dir.x * dir.x)),
        std::sqrt(1 + (dir.x * dir.x) / (dir.y * dir.y)));
    auto&& intercept = sf::Vector2f();

    if (dir.x < 0)
    {
        tileStep.x = -1;
        intercept.x = (pos.x - tile.x) * rayStep.x;
    }
    else
    {
        tileStep.x = 1;
        intercept.x = (tile.x + 1 - pos.x) * rayStep.x;
    }

    if (dir.y < 0)
    {
        tileStep.y = -1;
        intercept.y = (pos.y - tile.y) * rayStep.y;
    }
    else
    {
        tileStep.y = 1;
        intercept.y = (tile.y + 1 - pos.y) * rayStep.y;
    }

    bool side = 0;
    unsigned tileId = 0;
    bool lowPlaneTracker = false;                    // false if floor
    bool upperPlaneTracker = map.upperMesh.at(tile); // true if low ceiling
    float bottomHitDistance = 0.f;
    while (true)
    {
        tileId = tile.y * map.width + tile.x;

        unsigned realSide =
            side == 1 ? (dir.y > 0 ? 0 : 2) : (dir.x > 0 ? 3 : 1);
        unsigned visitedFacesIndex =
            realSide * map.width * map.heightHint + tileId;

        if (!lowPlaneTracker)
        {
            lowPlaneTracker = map.bottomMesh[tileId];
            if (lowPlaneTracker)
            {
                const auto interceptSize =
                    getInterceptSize(side, intercept, rayStep);
                bottomHitDistance = dgm::Math::vectorSize(computeHitloc(
                        pos, dir, getInterceptSize(side, intercept, rayStep))
                    - pos);
                tryAddFace(
                    visitedFaces,
                    faces,
                    visitedFacesIndex,
                    side,
                    realSide,
                    tileId,
                    sf::Vector2f(tile),
                    pos,
                    dir,
                    0.f,
                    interceptSize,
                    leftmostRay,
                    rightmostRay);
            }
        }

        bool upperTrackerNew = map.upperMesh.at(tile);
        if (upperTrackerNew && !upperPlaneTracker)
        {
            tryAddFace(
                visitedUpperFaces,
                upperFaces,
                visitedFacesIndex,
                side,
                realSide,
                tileId,
                sf::Vector2f(tile),
                pos,
                dir,
                1.f,
                getInterceptSize(side, intercept, rayStep),
                leftmostRay,
                rightmostRay);
        }
        upperPlaneTracker = upperTrackerNew;

        if (lowPlaneTracker && upperPlaneTracker) break;

        if (!visitedFlats[tileId])
        {
            visitedFlats[tileId] = true;
            addFlat(tile, pos, tileId, -0.5f);
            addFlat(tile, pos, tileId, upperPlaneTracker ? 0.5f : 1.5f);

            // Add sprites, ignore those that are fully obscured
            if (!lowPlaneTracker)
            {
                auto&& candidates = things.getOverlapCandidates(dgm::Rect(
                    sf::Vector2f(tile)
                        * sf::Vector2f(map.bottomMesh.getVoxelSize()).x,
                    sf::Vector2f(map.bottomMesh.getVoxelSize())));
                thingIds.insert(
                    thingIds.end(), candidates.begin(), candidates.end());
            }
        }

        if (intercept.x < intercept.y)
        {
            intercept.x += rayStep.x;
            tile.x += tileStep.x;
            side = 0; // vertical north-south
        }
        else
        {
            intercept.y += rayStep.y;
            tile.y += tileStep.y;
            side = 1; // horizontal west-east
        }
    }

    return bottomHitDistance;
}

void Raycaster::prepare()
{
    faces.clear();
    upperFaces.clear();
    flats.clear();
    thingIds.clear();
    visitedFaces.reset();
    visitedUpperFaces.reset();
    visitedFlats.reset();
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

void Raycaster::tryAddFace(
    VisitedFacesBitset& _visitedFaces,
    std::vector<Face>& _faces,
    unsigned visitedFacesIndex,
    unsigned side,
    unsigned realSide,
    unsigned tileId,
    sf::Vector2f tile,
    const sf::Vector2f& pos,
    const sf::Vector2f& dir,
    float heightHint,
    float interceptSize,
    bool leftmostRay,
    bool rightmostRay)
{
    if (!_visitedFaces[visitedFacesIndex])
    {
        _visitedFaces[visitedFacesIndex] = true;

        sf::Vector2f leftVertex, rightVertex;

        if (realSide == 0) // looking from north to south
        {
            leftVertex = { tile.x + 1.f, tile.y };
            rightVertex = tile;
        }
        else if (realSide == 1) // looking from east to west
        {
            leftVertex = { tile.x + 1.f, tile.y + 1.f };
            rightVertex = { tile.x + 1.f, tile.y };
        }
        else if (realSide == 2) // looking from south to north
        {
            leftVertex = { tile.x, tile.y + 1.f };
            rightVertex = { tile.x + 1.f, tile.y + 1.f };
        }
        else if (realSide == 3) // looking from east to west
        {
            leftVertex = tile;
            rightVertex = { tile.x, tile.y + 1.f };
        }

        _faces.push_back(Face { .leftVertex = leftVertex,
                                .rightVertex = rightVertex,
                                .side = side,
                                .tileId = tileId,
                                .distance = dgm::Math::vectorSize(
                                    (leftVertex + rightVertex) / 2.f - pos),
                                .heightHint = heightHint });
    }

    auto hitloc = computeHitloc(pos, dir, interceptSize);

    if (leftmostRay)
    {
        // Calling back because the face was just added, this is the first ray fired
        _faces.back().leftVertex = hitloc;
        _faces.back().leftTexHint = getTexHint(realSide, hitloc);
    }

    if (rightmostRay)
    {
        // Since the rightmostRay is the second one fired, the faces were just added
        _faces.back().rightVertex = hitloc;
        _faces.back().rightTexHint = getTexHint(realSide, hitloc);
    }
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
        .distance = dgm::Math::vectorSize((baseTile + baseTile2) / 2.f - pos),
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
}

sf::Vector2f Raycaster::computeHitloc(
    const sf::Vector2f& cameraPos,
    const sf::Vector2f& rayDir,
    float interceptSize) noexcept
{
    return cameraPos + dgm::Math::getUnitVector(rayDir) * interceptSize;
}
