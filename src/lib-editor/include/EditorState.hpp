#pragma once

#include <memory>
#include <map>
#include "Tools/Tool.hpp"

class EditorState {
private:
	std::unique_ptr<Tool> layer;

public:
	Tool& getTool() {
		return *layer;
	}

	const Tool& getTool() const {
		return *layer;
	}

	EditorState() {}
	EditorState(Tool *layer) : layer(layer) {}
};

class EditorStateManager {
private:
	std::map<std::string, EditorState> states;
	std::map<std::string, EditorState>::iterator currentState;
	std::vector<std::string> orderedByInsertion;

public:
	void addState(const std::string& id, Tool * layer);

	void changeState(const std::string& id);

	/**
	 *  \brief Loop over all states in order in which they were added
	 *
	 *  You need to provide callback that handles a tool reference
	 *  and a boolean flag indicating whether the particular tool
	 *  is active.
	 */
	void forallStates(std::function<void(Tool &, bool)> callback);

	/**
	 *  \brief Loop over all states, disregarding order
	 */
	void forallStates(std::function<void(Tool&)> callback);

	Tool & getTool() {
		return currentState->second.getTool();
	}

	const Tool & getTool() const {
		return currentState->second.getTool();
	}
};