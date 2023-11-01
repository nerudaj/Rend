#pragma once

#include "core/Scene.hpp"
#include <DGM/dgm.hpp>
#include <bitset>
#include <utils/RaycastingBase.hpp>

struct Face
{
    sf::Vector2f leftVertex;
    sf::Vector2f rightVertex;
    unsigned neighboringTileId; // passable tile from which the ray came
    unsigned tileId;
    float distance;
    float leftTexHint = 0.f;
    float rightTexHint = 1.f;
    float heightHint = 0.f;
};

struct Flat
{
    sf::Vector2f vertices[4];
    float heightHint;
    float distance;
    unsigned tileId;
};

struct Sprite
{
    unsigned objectId;
    sf::Vector2f leftVertex;
    sf::Vector2f rightVertex;
};

class Raycaster final : public RaycastingBase
{
private:
    using VisitedFacesBitset =
        std::bitset<MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT * 4>;

public:
    [[nodiscard]] Raycaster(const sf::Vector2u& voxelSize);

public:
    void prepare();

    /// <returns>Length of the ray</returns>
    template<bool LeftmostRay, bool RightmostRay>
    float castRay(
        const sf::Vector2f& pos, const sf::Vector2f& dir, const Scene& scene)
    {
        auto&& [tile, tileStep, rayStep, intercept] =
            computeInitialRaycastringStateFromPositionAndDirection(pos, dir);

        bool side = 0;
        unsigned tileId = tile.y * scene.level.width + tile.x, prevTileId = 0;
        bool lowPlaneTracker = false; // false if floor
        bool upperPlaneTracker =
            scene.level.upperMesh.at(tile); // true if low ceiling
        float bottomHitDistance = 0.f;
        while (true)
        {
            prevTileId = tileId;
            tileId = tile.y * scene.level.width + tile.x;

            const unsigned realSide =
                side == 1 ? (dir.y > 0 ? 0 : 2) : (dir.x > 0 ? 3 : 1);
            const unsigned visitedFacesIndex =
                realSide * scene.level.width * scene.level.heightHint + tileId;

            if (!lowPlaneTracker)
            {
                lowPlaneTracker = scene.level.bottomMesh[tileId];
                if (lowPlaneTracker)
                {
                    const auto interceptSize =
                        getInterceptSize(side, intercept, rayStep);
                    bottomHitDistance = dgm::Math::getSize(
                        computeHitloc(
                            pos,
                            dir,
                            getInterceptSize(side, intercept, rayStep))
                        - pos);
                    tryAddFace<LeftmostRay, RightmostRay, 0.f>(
                        visitedFaces,
                        faces,
                        visitedFacesIndex,
                        prevTileId,
                        realSide,
                        tileId,
                        sf::Vector2f(tile),
                        pos,
                        dir,
                        interceptSize);
                }
            }

            bool upperTrackerNew = scene.level.upperMesh.at(tile);
            if (upperTrackerNew && !upperPlaneTracker)
            {
                if constexpr (!RightmostRay)
                {
                    tryAddFace<LeftmostRay, false, 1.f>(
                        visitedUpperFaces,
                        upperFaces,
                        visitedFacesIndex,
                        prevTileId,
                        realSide,
                        tileId,
                        sf::Vector2f(tile),
                        pos,
                        dir,
                        getInterceptSize(side, intercept, rayStep));
                }
                else
                {
                    tryAddFace<false, true, 1.f>(
                        visitedUpperFaces,
                        upperFacesRight,
                        visitedFacesIndex,
                        prevTileId,
                        realSide,
                        tileId,
                        sf::Vector2f(tile),
                        pos,
                        dir,
                        getInterceptSize(side, intercept, rayStep));
                }
            }
            upperPlaneTracker = upperTrackerNew;

            if (lowPlaneTracker && upperPlaneTracker) break;

            if (!visitedFloors[tileId] && !lowPlaneTracker)
            {
                addFloorFlatAndThings(tile, pos, tileId, scene.spatialIndex);
            }

            if (!visitedCeils[tileId])
            {
                addCeilFlat(tile, pos, tileId, upperPlaneTracker);
            }

            side = advanceRay(intercept, tile, rayStep, tileStep);
        }

        if constexpr (RightmostRay)
        {
            upperFaces.insert(
                upperFaces.end(),
                upperFacesRight.begin(),
                upperFacesRight.end());
        }

        return bottomHitDistance;
    }

    void finalize();

public: // getters
    [[nodiscard]] constexpr const std::vector<Face>&
    getBottomFaces() const noexcept
    {
        return faces;
    }

    [[nodiscard]] constexpr const std::vector<Flat>& getFlats() const noexcept
    {
        return flats;
    }

    [[nodiscard]] constexpr const auto& getVisibleThingIds() const noexcept
    {
        return thingIds;
    }

private:
    [[nodiscard]] std::pair<sf::Vector2f, sf::Vector2f>
    computeVertexCoordsFromTileCoord(
        unsigned realSide, const sf::Vector2f& tile) const noexcept;

    template<bool LeftmostRay, bool RightmostRay, float HeightHint>
    void tryAddFace(
        VisitedFacesBitset& _visitedFaces,
        std::vector<Face>& _faces,
        unsigned visitedFacesIndex,
        unsigned neighboringTileId,
        unsigned realSide,
        unsigned tileId,
        sf::Vector2f tile,
        const sf::Vector2f& pos,
        const sf::Vector2f& dir,
        float interceptSize)
    {
        if (!_visitedFaces[visitedFacesIndex])
        {
            const auto&& [leftVertex, rightVertex] =
                computeVertexCoordsFromTileCoord(realSide, tile);
            const auto distance =
                dgm::Math::getSize((leftVertex + rightVertex) / 2.f - pos);

            _faces.push_back(Face { .leftVertex = leftVertex,
                                    .rightVertex = rightVertex,
                                    .neighboringTileId = neighboringTileId,
                                    .tileId = tileId,
                                    .distance = distance,
                                    .heightHint = HeightHint });
            _visitedFaces[visitedFacesIndex] = true;
        }

        auto hitloc = computeHitloc(pos, dir, interceptSize);

        if (_faces.empty()) [[unlikely]]
            return;

        if constexpr (LeftmostRay)
        {
            // Calling back because the face was just added, this is the first
            // ray fired
            _faces.back().leftVertex = hitloc;
            _faces.back().leftTexHint = getTexHint(realSide, hitloc);
        }

        if constexpr (RightmostRay)
        {
            // Since the rightmostRay is the second one fired, the faces were
            // just added
            _faces.back().rightVertex = hitloc;
            _faces.back().rightTexHint = getTexHint(realSide, hitloc);
        }
    }

    void addFloorFlatAndThings(
        const sf::Vector2u& tile,
        const sf::Vector2f& pos,
        unsigned tileId,
        const dgm::SpatialIndex<EntityIndexType>& things);

    void addCeilFlat(
        const sf::Vector2u& tile,
        const sf::Vector2f& pos,
        unsigned tileId,
        bool upperPlaneTracker);

    void addFlat(
        const sf::Vector2u& tile,
        const sf::Vector2f& pos,
        const unsigned tileId,
        const float heightHint);

    float
    getTexHint(unsigned realSide, const sf::Vector2f& hitloc) const noexcept;

private:
    std::vector<Face> faces;
    std::vector<Face> upperFaces;
    std::vector<Face> upperFacesRight;
    std::vector<Flat> flats;
    VisitedFacesBitset visitedFaces;      // map resolution * four sides
    VisitedFacesBitset visitedUpperFaces; // map resolution * four sides
    std::vector<decltype(Scene::spatialIndex)::IndexListType::value_type>
        thingIds;
    std::bitset<MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT> visitedFloors;
    std::bitset<MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT> visitedCeils;
};