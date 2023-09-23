#include "core/Scene.hpp"
#include "utils/Builder.hpp"
#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <input/NullController.hpp>
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

static auto createGameEntity(const LevelD::Thing& thing)
{
    using enum LeveldItemId;

    const auto id = static_cast<LeveldItemId>(thing.id);
    auto&& hitbox = dgm::Circle(getThingPosition(thing), THING_RADIUSES.at(id));

    switch (id)
    {
    case Medikit:
        return Entity {
            .typeId = EntityType::PickupHealth,
            .spriteClipIndex = SpriteId::MedikitA,
            .hitbox = hitbox,
        };
    case ArmorShard:
        return Entity {
            .typeId = EntityType::PickupArmor,
            .spriteClipIndex = SpriteId::ArmorShardA,
            .hitbox = hitbox,
        };
    case MegaHealth:
        return Entity {
            .typeId = EntityType::PickupMegaHealth,
            .spriteClipIndex = SpriteId::MegaHealthA,
            .hitbox = hitbox,
        };
    case MegaArmor:
        return Entity {
            .typeId = EntityType::PickupMegaArmor,
            .spriteClipIndex = SpriteId::MegaArmorA,
            .hitbox = hitbox,
        };
    case Bullets:
        return Entity {
            .typeId = EntityType::PickupBullets,
            .spriteClipIndex = SpriteId::BulletsA,
            .hitbox = hitbox,
        };
    case Shells:
        return Entity {
            .typeId = EntityType::PickupShells,
            .spriteClipIndex = SpriteId::ShellsA,
            .hitbox = hitbox,
        };
    case EnergyPack:
        return Entity {
            .typeId = EntityType::PickupEnergy,
            .spriteClipIndex = SpriteId::EnergyPackA,
            .hitbox = hitbox,
        };
    case Rockets:
        return Entity {
            .typeId = EntityType::PickupRockets,
            .spriteClipIndex = SpriteId::RocketsA,
            .hitbox = hitbox,
        };
    case Shotgun:
        return Entity {
            .typeId = EntityType::PickupShotgun,
            .spriteClipIndex = SpriteId::ShotgunA,
            .hitbox = hitbox,
        };
    case Pillar:
        return Entity {
            .typeId = EntityType::StaticDecoration,
            .spriteClipIndex = SpriteId::PillarA,
            .hitbox = hitbox,
        };
    case FloorLamp:
        return Entity {
            .typeId = EntityType::StaticDecoration,
            .spriteClipIndex = SpriteId::FloorLampA,
            .hitbox = hitbox,
        };
    default:
        throw dgm::Exception(std::format("Unknown thing id {}", thing.id));
    }
}

static auto createThingsBuffer(const LevelD& level)
{
    auto&& result = dgm::DynamicBuffer<Entity>();

    for (auto&& thing : level.things)
    {
        if (isSpawn(thing)) continue;
        result.emplaceBack(createGameEntity(thing));
    }

    return result;
}

static dgm::SpatialBuffer<Entity> createSpatialBuffer(const LevelD& level)
{
    auto&& result = dgm::SpatialBuffer<Entity>(
        { 0.f,
          0.f,
          static_cast<float>(level.mesh.layerWidth * level.mesh.tileWidth),
          static_cast<float>(level.mesh.layerHeight * level.mesh.tileHeight) },
        level.mesh.layerWidth);

    return result;
}

Scene Scene::buildScene(
    const dgm::ResourceManager& resmgr,
    const sf::Vector2f& baseResolution,
    const Settings& settings)
{
    auto&& level =
        resmgr.get<LevelD>(settings.cmdSettings.mapname).value().get();

    return Scene {
        .worldCamera = dgm::Camera(FULLSCREEN_VIEWPORT, baseResolution),
        .hudCamera = dgm::Camera(FULLSCREEN_VIEWPORT, baseResolution),
        .things = createThingsBuffer(level),
        .level = { .width = level.mesh.layerWidth,
                   .heightHint = level.mesh.layerHeight,
                   .bottomMesh = Builder::buildMeshFromLvd(level, 0),
                   .upperMesh = Builder::buildMeshFromLvd(level, 1) },
        .spatialIndex = createSpatialBuffer(level),
        .spawns = createSpawns(level),
        .mapname = settings.cmdSettings.mapname
    };
}

Entity Scene::createPlayer(
    const Position& position, const Direction& lookDirection, short inputId)
{
    return Entity { .typeId = EntityType::Player,
                    .spriteClipIndex = SpriteId::PlayerA0,
                    .hitbox = dgm::Circle(position.value, PLAYER_RADIUS),
                    .direction = lookDirection.value,
                    .inputId = inputId,
                    .health = PLAYER_INITIAL_HEALTH,
                    .armor = PLAYER_INITIAL_ARMOR,
                    .bulletCount = PLAYER_INITIAL_BULLETS,
                    .shellCount = PLAYER_INITIAL_NONBULLET_AMMO,
                    .energyCount = PLAYER_INITIAL_NONBULLET_AMMO,
                    .rocketCount = PLAYER_INITIAL_NONBULLET_AMMO };
}
