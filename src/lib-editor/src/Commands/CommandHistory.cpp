#include "Commands/CommandHistory.hpp"

void CommandHistory::add(const mem::Rc<UndoableCommandInterface>& command)
{
    if (index < commands.size())
    {
        commands.erase(commands.begin() + index, commands.end());
    }

    commands.push_back(command);
    index++;
}

void CommandHistory::undo()
{
    if (index == 0) return;

    index--;
    commands[index]->getInverse()->exec();
}

void CommandHistory::redo()
{
    if (index == commands.size()) return;

    commands[index]->exec();
    index++;
}
