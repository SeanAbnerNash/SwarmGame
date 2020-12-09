#include "stdafx.h"
#include "MenuSelectButtonCommand.h"

void MenuSelectButtonCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(MenuButtonPressed{ ButtonType::A });
}
