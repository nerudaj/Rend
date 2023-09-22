#include "engine/PhysicsEngine.hpp"
#include <utility>

void PhysicsEngine::update(const dgm::Time& time)
{
    auto&& playerHandler = [&](Player& actor, std::size_t id)
    {
        scene.things.removeFromLookup(id, actor.body);

        actor.direction = dgm::Math::getRotated(
            actor.direction,
            actor.input->getSteer() * PLAYER_RADIAL_SPEED
                * time.getDeltaTime());

        auto&& forward = dgm::Math::toUnit(
                             actor.direction * actor.input->getThrust()
                             + getPerpendicular(actor.direction)
                                   * actor.input->getSidewardThrust())
                         * PLAYER_FORWARD_SPEED * time.getDeltaTime();
        dgm::Collision::advanced(scene.level.bottomMesh, actor.body, forward);
        actor.body.move(forward);

        scene.things.returnToLookup(id, actor.body);
    };

    for (auto&& [thing, id] : scene.things)
    {
        std::visit(
            overloaded { [&](Player& p) { playerHandler(p, id); },
                         [](BaseObject&) {} },
            thing);
    }
}
