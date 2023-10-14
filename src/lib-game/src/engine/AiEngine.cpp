#include <core/EntityTraits.hpp>
#include <engine/AiEngine.hpp>

AiEngine::AiEngine(Scene& scene) : scene(scene), navmesh(scene.level.bottomMesh)
{
#define BIND(f) std::bind(&AiEngine::f, this, std::placeholders::_1)

    using dgm::fsm::decorator::Not;

    // clang-format off
    fsm = dgm::fsm::Builder<AiBlackboard, AiState>()
              .with(AiState::Start)
                  .when(Not<AiBlackboard>(BIND(isPlayerAlive))).goTo(AiState::WaitForRespawnRequest)
                  .otherwiseExec(BIND(doNothing)).andLoop()
              .with(AiState::WaitForRespawnRequest)
                  .exec(BIND(doNothing)).andGoTo(AiState::RequestRespawn)
              .with(AiState::RequestRespawn)
                  .exec(BIND(shoot)).andGoTo(AiState::WaitForRespawn)
              .with(AiState::WaitForRespawn)
                  .when(BIND(isPlayerAlive)).goTo(AiState::Start)
                  .otherwiseExec(BIND(doNothing)).andLoop()
              .build();
    // clang-format on

#undef BIND

    discoverInterestingLocation();
}

void AiEngine::update(const float deltaTime)
{
    for (auto&& state : scene.playerStates)
    {
        if (!state.blackboard.has_value()) continue;

        auto& blackboard = state.blackboard.value();
        auto& inventory = getInventory(blackboard);
        blackboard.input->clearInputs();
        fsm.setState(inventory.aiState);
        fsm.update(blackboard);
        inventory.aiState = fsm.getState();
    }
}

void AiEngine::discoverInterestingLocation()
{
    for (auto&& [entity, idx] : scene.things)
    {
        if (!isPickable(entity.typeId)) continue;
        if (isWeaponPickable(entity.typeId))
        {
            //            weaponLocations.emplace_back(
            //                weaponTypeToIndex(entity.typeId),
            //                entity.hitbox.getPosition());
        }
        else if (isPowerItemPickable(entity.typeId))
        {
            powerItemLocations.emplace_back(
                entity.typeId, entity.hitbox.getPosition());
        }
        else
        {
            pickupLocations.emplace_back(
                entity.typeId, entity.hitbox.getPosition());
        }
    }
}

void AiEngine::pickDestination(AiBlackboard& blackboard)
{
    const auto& inventory = getInventory(blackboard);

    // Pick destination
    [&]()
    {
        // First look for available weapons
        for (auto&& location : weaponLocations)
        {
            if (!inventory.acquiredWeapons[location.weaponIndex])
            {
                blackboard.targetLocation = location.position;
                return;
            }
        }

        // Second, look for power items
        for (auto&& location : powerItemLocations)
        {
            blackboard.targetLocation = location.position;
            // TODO: unless we are already here, pickup no longer exists, etc
            return;
        }
    }();

    const auto& player = getPlayer(blackboard);
    const auto& path =
        navmesh.getPath(player.hitbox.getPosition(), blackboard.targetLocation);
    blackboard.nextStop = path.getCurrentPoint().coord;
}

void AiEngine::goToDestination(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);
    const auto directionToDestination =
        blackboard.nextStop - player.hitbox.getPosition();
    const auto lookDirectionRelativeThrust =
        dgm::Math::toUnit(dgm::Math::getRotated(
            directionToDestination,
            -dgm::Math::cartesianToPolar(player.direction).angle));
    blackboard.input->setThrust(lookDirectionRelativeThrust.y);
    blackboard.input->setSidewardThrust(lookDirectionRelativeThrust.x);
}
