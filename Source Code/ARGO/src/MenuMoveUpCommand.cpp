#include "stdafx.h"
#include "MenuMoveUpCommand.h"

void MenuMoveUpCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(MenuMoveBetweenUI{ MoveDirection::Up });
}
