#include <core/EntityTraits.hpp>
#include <engine/AiEngine.hpp>

AiEngine::AiEngine(Scene& scene) : scene(scene), navmesh(scene.level.bottomMesh)
{
#define BIND(f) std::bind(&AiEngine::f, this, std::placeholders::_1)

    using dgm::fsm::decorator::Not;

    auto isPlayerDead = Not<AiBlackboard>(BIND(isPlayerAlive));

    // clang-format off
    fsm = dgm::fsm::Builder<AiBlackboard, AiState>()
              .with(AiState::Start)
                  .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
                  .otherwiseExec(BIND(doNothing)).andGoTo(AiState::PickNextJumpPoint)
              .with(AiState::PickNextJumpPoint)
                  .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
                  .otherwiseExec(BIND(pickJumpPoint)).andGoTo(AiState::WalkToJumpPoint)
              .with(AiState::WalkToJumpPoint)
                  .when(isPlayerDead).goTo(AiState::WaitForRespawnRequest)
                  .orWhen(BIND(isJumpPointReached)).goTo(AiState::PickNextJumpPoint)
                  .otherwiseExec(BIND(goToJumpPoint)).andLoop()
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

void AiEngine::pickJumpPoint(AiBlackboard& blackboard)
{
    const auto& inventory = getInventory(blackboard);

    // Pick destination
    [&]()
    {
        // First look for available weapons
        /*for (auto&& location : weaponLocations)
        {
            if (!inventory.acquiredWeapons[location.weaponIndex])
            {
                blackboard.targetLocation = location.position;
                return;
            }
        }*/

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
    if (path.isTraversed()) return;
    blackboard.nextStop = path.getCurrentPoint().coord;
}

bool AiEngine::isJumpPointReached(const AiBlackboard& blackboard) const
{
    const auto& player = getPlayer(blackboard);
    return dgm::Math::getSize(player.hitbox.getPosition() - blackboard.nextStop)
           < 2.f;
}

void AiEngine::goToJumpPoint(AiBlackboard& blackboard)
{
    const auto& player = getPlayer(blackboard);
    // todo: only convert to unit if bigger than one
    const auto directionToDestination =
        dgm::Math::toUnit(blackboard.nextStop - player.hitbox.getPosition());
    const float angle = dgm::Math::cartesianToPolar(player.direction).angle;
    const auto lookDirectionRelativeThrust =
        dgm::Math::getRotated(directionToDestination, -angle);
    blackboard.input->setThrust(lookDirectionRelativeThrust.x);
    blackboard.input->setSidewardThrust(lookDirectionRelativeThrust.y);
}
