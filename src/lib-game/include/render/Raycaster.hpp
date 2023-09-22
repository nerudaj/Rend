#pragma once

#include "core/Scene.hpp"
#include <DGM/dgm.hpp>
#include <bitset>

struct Face
{
    sf::Vector2f leftVertex;
    sf::Vector2f rightVertex;
    unsigned side;
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

class Raycaster final
{
private:
    using VisitedFacesBitset = std::bitset<MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT * 4>;

public:
    [[nodiscard]] Raycaster();

public:
    void prepare();

    /// <returns>Length of the ray</returns>
    float castRay(
        const sf::Vector2f& pos,
        const sf::Vector2f& dir,
        const Level& map,
        const dgm::SpatialBuffer<GameObject>& things,
        bool leftmostRay,
        bool rightmostRay);

    void finalize();

public: // getters
    [[nodiscard]] constexpr const std::vector<Face>& getBottomFaces() const noexcept
    {
        return faces;
    }

    [[nodiscard]] constexpr const std::vector<Flat>& getFlats() const noexcept
    {
        return flats;
    }

    [[nodiscard]] constexpr const auto& getVisibleThingIds() const noexcept {
        return thingIds;
    }

private:
    void tryAddFace(
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
        bool rightmostRay);

    void addFlat(
        const sf::Vector2u& tile,
        const sf::Vector2f& pos,
        const unsigned tileId,
        const float heightHint);

    float
    getTexHint(unsigned realSide, const sf::Vector2f& hitloc) const noexcept;

    static sf::Vector2f computeHitloc(
        const sf::Vector2f& cameraPos,
        const sf::Vector2f& rayDir,
        float interceptSize) noexcept;

private:
    std::vector<Face> faces;
    std::vector<Face> upperFaces;
    std::vector<Flat> flats;
    VisitedFacesBitset visitedFaces;      // map resolution * four sides
    VisitedFacesBitset visitedUpperFaces; // map resolution * four sides
    std::vector<decltype(Scene::things)::IndexListType::value_type> thingIds;
    std::bitset<MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT> visitedFlats;
};