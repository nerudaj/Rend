#include "include/Editor/EditorStateManager.hpp"

void EditorStateManager::forallStates(std::function<void(ToolInterface&)> callback)
{
	for (auto&& stateId : insertionOrder)
	{
		callback(*statesToTools.at(stateId));
	}
}

void EditorStateManager::forallStates(std::function<void(const ToolInterface&)> callback) const
{
	for (auto&& stateId : insertionOrder)
	{
		callback(*statesToTools.at(stateId));
	}
}

void EditorStateManager::forallStates(std::function<void(ToolInterface&, bool)> callback)
{
	for (auto&& stateId : insertionOrder)
	{
		callback(*statesToTools.at(stateId), stateId == currentState);
	}
}

void EditorStateManager::forallInactiveStates(std::function<void(ToolInterface&)> callback)
{
	for (auto&& stateId : insertionOrder)
	{
		if (stateId == currentState)
			continue;
		callback(*statesToTools.at(stateId));
	}
}

void EditorStateManager::changeState(EditorState state)
{
	assert(statesToTools.contains(state));
	currentState = state;
}
