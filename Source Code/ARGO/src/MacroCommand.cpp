#include "stdafx.h"
#include "MacroCommand.h"

MacroCommand::MacroCommand()
{
}

void MacroCommand::add(Command* t_command)
{
	m_commands.push(t_command);
	m_commandHistory.push(t_command);
}

void MacroCommand::addAndExecute(Command* t_command, Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	add(t_command);
	executeTop(t_entity, t_eventManager, t_dt);
}

void MacroCommand::executeTop(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	m_commands.top()->execute(t_entity, t_eventManager, t_dt);
}

void MacroCommand::executeTopAndPop(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	executeTop(t_entity, t_eventManager, t_dt);
	popTopCommand();
}

std::stack<Command*> MacroCommand::getCommands()
{
	return m_commands;
}

void MacroCommand::popTopCommand()
{
	m_commands.pop();
}
