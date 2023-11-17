#pragma once

#include "Commands/CommandHelper.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include "Utilities/InitGuard.hpp"
#include <LevelD.hpp>
#include <vector>

template<TriggerOrThing>
class CreateObjectCommand;

template<TriggerOrThing ObjectType>
class DeleteObjectCommand final : public UndoableCommandInterface
{
protected:
    std::vector<ObjectType>& target;
    std::vector<std::size_t> idsToDelete;
    InitGuard<std::vector<ObjectToCreate<ObjectType>>> objectsToCreate;

public:
    [[nodiscard]] DeleteObjectCommand(
        std::vector<ObjectType>& target,
        const std::vector<std::size_t>& idsToDelete) noexcept
        : target(target), idsToDelete(idsToDelete)
    {
    }

public:
    void exec() override
    {
        objectsToCreate =
            CommandHelper::deleteObjectsFromTarget(target, idsToDelete);
    }

    [[nodiscard]] mem::Box<CommandInterface> getInverse() const override
    {
        return mem::Box<CreateObjectCommand<ObjectType>>(
            target, *objectsToCreate);
    }
};

template<TriggerOrThing ObjectType>
class CreateObjectCommand final : public UndoableCommandInterface
{
protected:
    std::vector<ObjectType>& target;
    std::vector<ObjectToCreate<ObjectType>> objectsToCreate;
    InitGuard<std::vector<std::size_t>> idsOfAddedObjects;

public:
    [[nodiscard]] CreateObjectCommand(
        std::vector<ObjectType>& target,
        const ObjectType& objectToCreate) noexcept
        : target(target)
    {
        objectsToCreate.push_back(ObjectToCreate<ObjectType> {
            .arrayPosition = target.size(), .object = objectToCreate });
    }

    // Used by DeleteObjectCommand
    CreateObjectCommand(
        std::vector<ObjectType>& target,
        const std::vector<ObjectToCreate<ObjectType>>& objectsToCreate) noexcept
        : target(target), objectsToCreate(objectsToCreate)
    {
    }

public:
    void exec() override
    {
        idsOfAddedObjects =
            CommandHelper::addObjectsToTarget(target, objectsToCreate);
    }

    [[nodiscard]] mem::Box<CommandInterface> getInverse() const override
    {
        return mem::Box<DeleteObjectCommand<ObjectType>>(
            target, *idsOfAddedObjects);
    }
};

typedef CreateObjectCommand<LevelD::Thing> CreateItemCommand;

typedef DeleteObjectCommand<LevelD::Thing> DeleteItemCommand;

typedef CreateObjectCommand<LevelD::Trigger> CreateTriggerCommand;

typedef DeleteObjectCommand<LevelD::Trigger> DeleteTriggerCommand;
