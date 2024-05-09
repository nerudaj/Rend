#pragma once

#include "Commands/CommandHelper.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include "Utilities/InitGuard.hpp"
#include <LevelD.hpp>
#include <Memory.hpp>
#include <SFML/System/Vector2.hpp>

template<TriggerOrThing ObjectType>
class SetObjectPropertyCommand : public UndoableCommandInterface
{
public:
    SetObjectPropertyCommand(
        std::vector<ObjectType>& target,
        unsigned id,
        const ObjectType& data,
        const sf::Vector2u& levelSize)
        : target(target), objectId(id), data(data), levelSize(levelSize)
    {
    }

public:
    virtual void exec() override
    {
        oldData = target[objectId];
        target[objectId] = data;
        target[objectId].x =
            std::clamp<unsigned>(target[objectId].x, 0u, levelSize.x);
        target[objectId].y =
            std::clamp<unsigned>(target[objectId].y, 0u, levelSize.y);
    }

    virtual mem::Box<CommandInterface> getInverse() const override
    {
        return mem::Box<SetObjectPropertyCommand>(
            target, objectId, *oldData, levelSize);
    }

protected:
    std::vector<ObjectType>& target;
    unsigned objectId;
    ObjectType data;
    sf::Vector2u levelSize;
    InitGuard<ObjectType> oldData;
};
