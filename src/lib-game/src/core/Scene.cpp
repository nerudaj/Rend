#include "core/Scene.hpp"
#include "utils/Builder.hpp"
#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <input/PhysicalController.hpp>
#include <tuple>

static inline const sf::FloatRect FULLSCREEN_VIEWPORT = { 0.f, 0.f, 1.f, 1.f };

static constexpr bool isSpawn(const LevelD::Thing& thing) noexcept
{
    return static_cast<LeveldItemId>(thing.id) == LeveldItemId::PlayerSpawn;
}

static sf::Vector2f getThingPosition(const LevelD::Thing& thing) noexcept
{
    return sf::Vector2f(
        static_cast<float>(thing.x), static_cast<float>(thing.y));
}

static std::vector<sf::Vector2f> createSpawns(const LevelD& level)
{
    return level.things | std::views::filter(isSpawn)
           | std::views::transform(getThingPosition)
           | std::ranges::to<std::vector<sf::Vector2f>>();
}

static GameObject createGameObject(LeveldItemId id, const LevelD::Thing& item)
{
    using enum LeveldItemId;

    auto&& body = dgm::Circle(getThingPosition(item), THING_RADIUSES.at(id));

    switch (id)
    {
    case Medikit:
        return GenericPickup(body, SpriteId::MedikitA, PickupId::Medkit);
    case ArmorShard:
        return GenericPickup(body, SpriteId::ArmorShardA, PickupId::ArmorShard);
    case MegaHealth:
        return GenericPickup(body, SpriteId::MegaHealthA, PickupId::MegaHealth);
    case MegaArmor:
        return GenericPickup(body, SpriteId::MegaArmorA, PickupId::MegaArmor);
    case Bullets:
        return GenericPickup(body, SpriteId::BulletsA, PickupId::Bullets);
    case Shells:
        return GenericPickup(body, SpriteId::ShellsA, PickupId::Shells);
    case EnergyPack:
        return GenericPickup(body, SpriteId::EnergyPackA, PickupId::EnergyPack);
    case Rockets:
        return GenericPickup(body, SpriteId::RocketsA, PickupId::Rockets);
    case Shotgun:
        return WeaponPickup(body, SpriteId::ShotgunA, WeaponId::Shotgun);
    case Pillar:
        return CollidableObject(body, SpriteId::PillarA);
    case FloorLamp:
        return CollidableObject(body, SpriteId::FloorLampA);
    default:
        throw std::exception();
    };
}

static dgm::SpatialBuffer<GameObject> createSpatialBuffer(const LevelD& level)
{
    auto&& result = dgm::SpatialBuffer<GameObject>(
        { 0.f,
          0.f,
          static_cast<float>(level.mesh.layerWidth * level.mesh.tileWidth),
          static_cast<float>(level.mesh.layerHeight * level.mesh.tileHeight) },
        level.mesh.layerWidth);

    for (auto&& thing : level.things)
    {
        if (isSpawn(thing)) continue;

        try
        {
            auto&& object =
                createGameObject(static_cast<LeveldItemId>(thing.id), thing);
            std::visit([&](auto o) { result.insert(o, o.body); }, object);
        }
        catch (...)
        {
        }
    }

    return result;
}

Scene Scene::buildScene(
    const dgm::ResourceManager& resmgr,
    const sf::Vector2f& baseResolution,
    const Settings& settings)
{
    auto&& level = resmgr.get<LevelD>(settings.cmdSettings.mapname).value().get();

    return Scene {
        .worldCamera = dgm::Camera(FULLSCREEN_VIEWPORT, baseResolution),
        .hudCamera = dgm::Camera(FULLSCREEN_VIEWPORT, baseResolution),
        .level = { .width = level.mesh.layerWidth,
                   .heightHint = level.mesh.layerHeight,
                   .bottomMesh = Builder::buildMeshFromLvd(level, 0),
                   .upperMesh = Builder::buildMeshFromLvd(level, 1) },
        .things = createSpatialBuffer(level),
        .spawns = createSpawns(level),
        .mapname = settings.cmdSettings.mapname
    };
}
