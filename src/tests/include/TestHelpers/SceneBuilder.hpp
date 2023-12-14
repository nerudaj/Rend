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
    return Scene { .spatialIndex = dgm::SpatialIndex<EntityIndexType>(
                       dgm::Rect(0.f, 0.f, 48.f, 48.f), 16),
                   .distanceIndex = DistanceIndex(mesh),
                   .navmesh = dgm::WorldNavMesh(mesh) };
}
