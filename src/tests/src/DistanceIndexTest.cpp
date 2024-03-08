#include <catch.hpp>
#include <limits>
#include <print>
#include <utils/DistanceIndex.hpp>

TEST_CASE("[DistanceIndex]")
{
    /*
    Works on this map:
    ######
    #    #
    # ## #
    # #  #
    #  # #
    #  # #
    ######
    */

    // clang-format off
    auto&& mesh = dgm::Mesh(
        std::vector<bool> {  true,  true,  true,  true,  true,  true,
                             true, false, false, false, false,  true,
                             true, false,  true,  true, false,  true,
                             true, false,  true,  true, false,  true,
                             true, false, false,  true, false,  true,
                             true, false, false,  true, false,  true,
                             true,  true,  true,  true,  true,  true},
        { 6u, 7u },
        { 16u, 16u });
    // clang-format on
    auto&& index = DistanceIndex(mesh);

    auto toCoord = [&mesh](unsigned x, unsigned y)
    {
        return sf::Vector2f(
                   x * mesh.getVoxelSize().x, y * mesh.getVoxelSize().y)
               + sf::Vector2f(mesh.getVoxelSize()) / 2.f;
    };

    SECTION("All distance pairs are correctly computed")
    {
        for (unsigned fromY = 1; fromY < mesh.getDataSize().y; ++fromY)
        {
            for (unsigned fromX = 1; fromX < mesh.getDataSize().x; ++fromX)
            {
                for (unsigned toY = 1; toY < mesh.getDataSize().y; ++toY)
                {
                    for (unsigned toX = 1; toX < mesh.getDataSize().y; ++toX)
                    {

                        const auto&& path = dgm::TileNavMesh::computePath(
                            { fromX, fromY }, { toX, toY }, mesh);
                        const auto&& indexedDistance = index.getDistance(
                            toCoord(fromX, fromY), toCoord(toX, toY));
                        std::println(
                            "[{}, {}] -> [{}, {}] = {} (indexed)]",
                            fromX,
                            fromY,
                            toX,
                            toY,
                            indexedDistance);

                        if (!path)
                        {
                            REQUIRE(
                                std::numeric_limits<
                                    DistanceIndex::DistanceType>::max()
                                == indexedDistance);
                        }
                        else
                        {
                            REQUIRE(
                                path.value().getLength() == indexedDistance);
                        }
                    }
                }
            }
        }
    }

    SECTION("Trying to get distance to a wall yields huge number")
    {
        REQUIRE(
            index.getDistance({ 24.f, 24.f }, { 8.f, 8.f })
            == std::numeric_limits<DistanceIndex::DistanceType>::max());
        REQUIRE(
            index.getDistance({ 40.f, 40.f }, { 8.f, 70.f })
            == std::numeric_limits<DistanceIndex::DistanceType>::max());
    }
}