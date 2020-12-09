#include "stdafx.h"
#include "GoToMainMenuCommand.h"

void GoToMainMenuCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(ChangeScreen{ MenuStates::MainMenu });
}
