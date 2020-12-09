#include "stdafx.h"
#include "GoToLicenseScreenCommand.h"

void GoToLicenseScreenCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(ChangeScreen{ MenuStates::License });
}
