#include <core/EntityTraits.hpp>
#include <engine/AiEngine.hpp>

AiEngine::AiEngine(Scene& scene) : scene(scene), navmesh(scene.level.bottomMesh)
{
#define BIND(f) std::bind(&AiEngine::f, this, std::placeholders::_1)

    // clang-format off
    fsm = dgm::fsm::Builder<AiBlackboard, AiState>()
              .with(AiState::Start)
                  .exec(BIND(doNothing)).andGoTo(AiState::PickDestination)
              .with(AiState::PickDestination)
                  .exec(BIND(pickDestination)).andGoTo(AiState::GoToDestination)
              .with(AiState::GoToDestination)
                  .when(BIND(isDestinationReached)).goTo(AiState::PickDestination)
                  .otherwiseExec(BIND(goToDestination)).andLoop()
              .with(AiState::WaitForRespawn)
                  .exec(BIND(doNothing)).andGoTo(AiState::RequestRespawn)
              .with(AiState::RequestRespawn)
                  .exec(BIND(shoot)).andGoTo(AiState::WaitForRespawn2)
              .with(AiState::WaitForRespawn2)
                  .exec(BIND(doNothing)).andGoTo(AiState::Start)
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

        if (!isPlayerAlive(blackboard)
            && inventory.aiState < AiState::WaitForRespawn)
        {
            inventory.aiState = AiState::WaitForRespawn;
        }

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
