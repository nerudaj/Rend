#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <builder/LightmapBuilder.hpp>
#include <builder/MeshBuilder.hpp>
#include <builder/SceneBuilder.hpp>
#include <core/Constants.hpp>
#include <core/EntityDefinitions.hpp>
#include <input/NullController.hpp>
#include <tuple>

static inline const sf::FloatRect FULLSCREEN_VIEWPORT = { 0.f, 0.f, 1.f, 1.f };

static constexpr bool isSpawn(const LevelD::Thing& thing) noexcept
{
    return static_cast<LevelItemId>(thing.id) == LevelItemId::PlayerSpawn;
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
convertLeveldItemIdToEntityType(LevelItemId id) noexcept
{
    using enum LevelItemId;
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
    case Trishot:
        return EntityType::PickupTrishot;
    case Crossbow:
        return EntityType::PickupCrossbow;
    case Launcher:
        return EntityType::PickupLauncher;
    case Ballista:
        return EntityType::PickupBallista;
    case Pillar:
        return EntityType::Pillar;
    case FloorLamp:
        return EntityType::FloorLamp;
    default:
        return EntityType::Error;
    }
}

[[nodiscard]] static auto
createEntity(EntityType typeId, const sf::Vector2f& position)
{
    const auto& props = ENTITY_PROPERTIES.at(typeId);
    return Entity { .typeId = typeId,
                    .animationContext { .spriteClipIndex =
                                            props.initialSpriteIndex },
                    .hitbox = dgm::Circle(position, props.radius) };
}

[[nodiscard]] static auto createEntity(const LevelD::Thing& thing)
{
    return createEntity(
        convertLeveldItemIdToEntityType(static_cast<LevelItemId>(thing.id)),
        getThingPosition(thing));
}

static void createCameraAnchorsAtPlaceOfFirstSpawn(
    const LevelD& level,
    dgm::DynamicBuffer<Entity>& result,
    unsigned maxPlayerCount)
{
    for (auto&& thing : level.things)
    {
        if (isSpawn(thing))
        {
            for (std::size_t idx : std::views::iota(0u, maxPlayerCount))
            {
                result.emplaceBack(createEntity(
                    EntityType::CameraAnchor,
                    sf::Vector2f(
                        static_cast<float>(thing.x),
                        static_cast<float>(thing.y))));
            }
            return;
        }
    }
}

static auto createThingsBuffer(const LevelD& level, unsigned maxPlayerCount)
{
    auto&& result = dgm::DynamicBuffer<Entity>();

    createCameraAnchorsAtPlaceOfFirstSpawn(level, result, maxPlayerCount);

    for (auto&& thing : level.things)
    {
        if (isSpawn(thing)) continue;
        result.emplaceBack(createEntity(thing));
    }

    return result;
}

static dgm::SpatialIndex<EntityIndexType>
createSpatialIndex(const LevelD& level)
{
    auto&& result = dgm::SpatialIndex<EntityIndexType>(
        { 0.f,
          0.f,
          static_cast<float>(level.mesh.layerWidth * level.mesh.tileWidth),
          static_cast<float>(level.mesh.layerHeight * level.mesh.tileHeight) },
        level.mesh.layerWidth);

    return result;
}

[[nodiscard]] dgm::WorldNavMesh
createNavMesh(dgm::Mesh collisionMesh, dgm::DynamicBuffer<Entity>& things)
{
    for (auto&& [thing, _] : things)
    {
        auto& def = ENTITY_PROPERTIES.at(thing.typeId);
        if (def.traits & Trait::Solid)
        {
            collisionMesh.at(
                sf::Vector2u(thing.hitbox.getPosition())
                / collisionMesh.getVoxelSize().x) = 1;
        }
    }

    return dgm::WorldNavMesh(collisionMesh);
}

Scene SceneBuilder::buildScene(const LevelD& level, unsigned maxPlayerCount)
{
    auto things = createThingsBuffer(level, maxPlayerCount);
    const auto bottomMesh = MeshBuilder::buildMeshFromLvd(level, 0);
    auto navmesh = createNavMesh(bottomMesh, things);
    const auto bottomTextureMesh =
        MeshBuilder::buildTextureMeshFromLvd(level, 0);
    const auto upperTextureMesh =
        MeshBuilder::buildTextureMeshFromLvd(level, 1);

    return Scene {
        .things = std::move(things),
        .level = { .width = level.mesh.layerWidth,
                   .heightHint = level.mesh.layerHeight,
                   .bottomMesh = bottomMesh,
                   .upperMesh = MeshBuilder::buildMeshFromLvd(level, 1) },
        .drawableLevel = { .bottomTextures = bottomTextureMesh,
                           .upperTextures = upperTextureMesh,
                           .lightmap = LightmapBuilder::buildLightmap(
                               bottomTextureMesh, upperTextureMesh, level) },
        .spatialIndex = createSpatialIndex(level),
        .distanceIndex = DistanceIndex(bottomMesh),
        .spawns = createSpawns(level),
        .navmesh = std::move(navmesh)
    };
}

Entity SceneBuilder::createPlayer(
    const Position& position,
    const Direction& lookDirection,
    PlayerStateIndexType stateIdx) noexcept
{
    const auto& eprops = ENTITY_PROPERTIES.at(EntityType::Player);

    return Entity {
        .typeId = EntityType::Player,
        .animationContext = {
            eprops.initialSpriteIndex,
        },
        .hitbox = dgm::Circle(
            position.value,
            eprops.radius),
        .direction = lookDirection.value,
        .stateIdx = stateIdx,
        .health = PLAYER_INITIAL_HEALTH,
        .armor = PLAYER_INITIAL_ARMOR,
    };
}

PlayerInventory
SceneBuilder::getDefaultInventory(EntityIndexType ownerIdx, int score) noexcept
{
    return PlayerInventory {
        .ownerIdx = ownerIdx,
        .activeWeaponType = EntityType::WeaponFlaregun,
        .lastWeaponType = EntityType::WeaponFlaregun,
        .animationContext { .spriteClipIndex =
                                ENTITY_PROPERTIES.at(EntityType::WeaponFlaregun)
                                    .initialSpriteIndex },
        .acquiredWeapons = 0b0000000000000001,
        .score = score
    };
}

Entity SceneBuilder::createProjectile(
    EntityType type,
    const Position& position,
    const Direction& direction,
    std::size_t frameIdx,
    PlayerStateIndexType originatorStateIdx) noexcept
{
    const auto& eprops = ENTITY_PROPERTIES.at(type);
    return Entity { .typeId = type,
                    .animationContext { .spriteClipIndex =
                                            eprops.initialSpriteIndex,
                                        .lastAnimationUpdate = frameIdx },
                    .hitbox = dgm::Circle(position.value, eprops.radius),
                    .direction = direction.value,
                    .stateIdx = originatorStateIdx,
                    .health = 100 };
}

Entity SceneBuilder::createEffect(
    EntityType type, const Position& position, std::size_t frameIdx)
{
    const auto& eprops = ENTITY_PROPERTIES.at(type);

    return Entity {
        .typeId = type,
        .animationContext = { .spriteClipIndex = eprops.initialSpriteIndex,
                              .lastAnimationUpdate = frameIdx },
        .hitbox = dgm::Circle(position.value, eprops.radius),
    };
}

Entity SceneBuilder::createPickup(EntityType typeId, const Position& position)
{
    const auto& props = ENTITY_PROPERTIES.at(typeId);
    return Entity { .typeId = typeId,
                    .animationContext {
                        .spriteClipIndex = props.initialSpriteIndex,
                    },
                    .hitbox = dgm::Circle(position.value, props.radius) };
}

Entity SceneBuilder::createDecal(
    bool isFlesh, const Position& position, std::size_t tick) noexcept
{
    const auto type = EntityType::Decal;
    const auto& props = ENTITY_PROPERTIES.at(type);
    const auto state =
        isFlesh ? AnimationStateId::Missile : AnimationStateId::Idle;
    return Entity {
        .typeId = type,
        .animationContext = { .spriteClipIndex =
                                  props.states.at(state).clip.front().spriteId,
                              .lastAnimationUpdate = tick },
        .hitbox = dgm::Circle(position.value, 1_px)
    };
}
