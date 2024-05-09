#pragma once

#include "Editor/EditorState.hpp"
#include "Interfaces/ToolInterface.hpp"
#include <Memory.hpp>
#include <functional>
#include <map>
#include <vector>

class [[nodiscard]] EditorStateManager final
{
private:
    std::map<EditorState, mem::Box<ToolInterface>> statesToTools;
    EditorState currentState = EditorState();
    std::vector<EditorState> insertionOrder;

public:
    template<DerivedFromTool T, class... Args>
        requires std::constructible_from<T, Args...>
    void addState(EditorState state, Args&&... args)
    {
        // overwrite is allowed
        if (statesToTools.contains(state))
        {
            statesToTools.erase(state);
        }
        else
        {
            insertionOrder.push_back(state);
        }

        statesToTools.emplace(std::make_pair(
            state,
            mem::Box<ToolInterface>(mem::Box<T>(std::forward<Args>(args)...))));
    }

    void changeState(EditorState state);

    [[nodiscard]] constexpr EditorState getCurrentState() const noexcept
    {
        return currentState;
    }

    /**
     *  \brief Loop over all states in order in which they were added
     */
    void forallStates(std::function<void(ToolInterface&)> callback);

    void forallStates(std::function<void(const ToolInterface&)> callback) const;

    /**
     *  \brief Loop over all states in order in which they were added
     *
     *  You need to provide callback that handles a tool reference
     *  and a boolean flag indicating whether the particular tool
     *  is active.
     */
    void forallStates(std::function<void(ToolInterface&, bool)> callback);

    void forallInactiveStates(std::function<void(ToolInterface&)> callback);

    [[nodiscard]] ToolInterface& getActiveTool() noexcept
    {
        return *statesToTools.at(currentState);
    }

    [[nodiscard]] const ToolInterface& getActiveTool() const noexcept
    {
        return *statesToTools.at(currentState);
    }

    [[nodiscard]] auto& getTool(this auto&& self, EditorState state) noexcept
    {
        return *self.statesToTools.at(state);
    }
};
