#include "stdafx.h"
#include "CloseWindowCommand.h"

void CloseWindowCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(CloseWindow());
}
