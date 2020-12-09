#include "stdafx.h"
#include "MenuMoveDownCommand.h"

void MenuMoveDownCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(MenuMoveBetweenUI{ MoveDirection::Down });
}
