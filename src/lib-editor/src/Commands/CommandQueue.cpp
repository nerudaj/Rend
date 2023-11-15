#include "Commands/CommandQueue.hpp"

void CommandQueue::processAll()
{
    while (!commands.empty())
    {
        auto command = commands.front();
        commands.pop();
        command->exec();
        history->add(command);
    }
}
