#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <input/NullController.hpp>
#include <tuple>
#include <utils/Builder.hpp>
#include <utils/SceneBuilder.hpp>

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

[[nodiscard]] static EntityType
convertLeveldItemIdToEntityType(LeveldItemId id) noexcept
{
    using enum LeveldItemId;
    switch (id)
    {
    case Medikit:
        return EntityType::PickupHealth;
    case ArmorShard:
        return EntityType::PickupArmor;
    case MegaHealth:
        return EntityType::PickupMegaHealth;
    case MegaArmor:
        return EntityType::PickupMegaArmor;
    case Bullets:
        return EntityType::PickupBullets;
    case Shells:
        return EntityType::PickupShells;
    case EnergyPack:
        return EntityType::PickupEnergy;
    case Rockets:
        return EntityType::PickupRockets;
    case Shotgun:
        return EntityType::PickupShotgun;
    case Pillar:
        return EntityType::Pillar;
    case FloorLamp:
        return EntityType::FloorLamp;
    default:
        return EntityType::Error;
    }
}

static auto createEntity(const LevelD::Thing& thing)
{
    const auto typeId =
        convertLeveldItemIdToEntityType(static_cast<LeveldItemId>(thing.id));
    const auto& props = ENTITY_PROPERTIES.at(typeId);
    const auto hitbox = dgm::Circle(getThingPosition(thing), props.radius);
    return Entity { .typeId = typeId,
                    .spriteClipIndex = props.initialSpriteIndex,
                    .hitbox = hitbox };
}

static auto createThingsBuffer(const LevelD& level)
{
    auto&& result = dgm::DynamicBuffer<Entity>();

    for (auto&& thing : level.things)
    {
        if (isSpawn(thing)) continue;
        result.emplaceBack(createEntity(thing));
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

Scene SceneBuilder::buildScene(
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

Entity SceneBuilder::createPlayer(
    const Position& position,
    const Direction& lookDirection,
    short inputId) noexcept
{
    return Entity { .typeId = EntityType::Player,
                    .spriteClipIndex = SpriteId::PlayerA0,
                    .hitbox = dgm::Circle(
                        position.value,
                        ENTITY_PROPERTIES.at(EntityType::Player).radius),
                    .direction = lookDirection.value,
                    .inputId = inputId,
                    .health = PLAYER_INITIAL_HEALTH,
                    .armor = PLAYER_INITIAL_ARMOR,
                    .bulletCount = PLAYER_INITIAL_BULLETS,
                    .shellCount = PLAYER_INITIAL_NONBULLET_AMMO,
                    .energyCount = PLAYER_INITIAL_NONBULLET_AMMO,
                    .rocketCount = PLAYER_INITIAL_NONBULLET_AMMO };
}

Entity SceneBuilder::createProjectile(
    EntityType type,
    const Position& position,
    const Direction& direction,
    std::size_t frameIdx) noexcept
{
    return Entity { .typeId = type,
                    .spriteClipIndex = SpriteId::RocketA0,
                    .hitbox = dgm::Circle(position.value, 2_px),
                    .direction = direction.value,
                    .lastAnimationUpdate = frameIdx };
}

Entity SceneBuilder::createEffect(
    EntityType type, const Position& position, std::size_t frameIdx)
{
    switch (type)
    {
        using enum EntityType;
        using enum SpriteId;
    case EffectExplosion:
        return Entity { .typeId = type,
                        .spriteClipIndex = ExplosionA,
                        .hitbox = dgm::Circle(position.value, 8_px),
                        .lastAnimationUpdate = frameIdx };
    case EffectDyingPlayer:
        return Entity { .typeId = type,
                        .spriteClipIndex = DeathA,
                        .hitbox = dgm::Circle(position.value, 8_px),
                        .lastAnimationUpdate = frameIdx };
    case EffectSpawn:
        return Entity { .typeId = type,
                        .spriteClipIndex = SpawnItemA,
                        .hitbox = dgm::Circle(position.value, 8_px),
                        .lastAnimationUpdate = frameIdx };
    default:
        throw dgm::Exception(std::format("Unknown effect id {}", int(type)));
    }
}

Entity SceneBuilder::createPickup(EntityType typeId, const Position& position)
{
    const auto& props = ENTITY_PROPERTIES.at(typeId);
    return Entity { .typeId = typeId,
                    .spriteClipIndex = props.initialSpriteIndex,
                    .hitbox = dgm::Circle(position.value, props.radius) };
}
