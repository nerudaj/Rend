#include <builder/SceneBuilder.hpp>
#include <core/Scene.hpp>

static [[nodiscard]] dgm::Mesh createDummyMesh()
{
    auto&& meshSize = sf::Vector2u(3u, 3u);
    auto&& voxelSize = sf::Vector2u(16u, 16u);
    return dgm::Mesh(
        std::vector<int> { 1, 1, 1, 1, 0, 1, 1, 1, 1 }, meshSize, voxelSize);
}

static [[nodiscard]] Scene createDummyScene(const dgm::Mesh& mesh)
{
    return Scene { .level = { .width = mesh.getDataSize().x,
                              .height = mesh.getDataSize().y,
                              .bottomMesh = mesh,
                              .upperMesh = mesh },
                   .drawableLevel = { .bottomTextures = mesh,
                                      .upperTextures = mesh,
                                      .lightmap = LightmapType(
                                          std::vector<LightType> {},
                                          { 0u, 0u },
                                          { 0u, 0u }) },
                   .spatialIndex = dgm::SpatialIndex<EntityIndexType>(
                       dgm::Rect(0.f, 0.f, 48.f, 48.f), 16),
                   .distanceIndex = DistanceIndex(mesh),
                   .navmesh = dgm::WorldNavMesh(mesh) };
}

static [[nodiscard]] sf::Vector2f getSafePosition()
{
    return sf::Vector2f(24.f, 24.f);
}

static [[nodiscard]] Entity createDummyPlayer()
{
    return SceneBuilder::createPlayer(
        Position { getSafePosition() },
        Direction { sf::Vector2f(1.f, 0.f) },
        0u);
}