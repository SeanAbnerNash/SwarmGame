#include "stdafx.h"
#include "WeaponCycleDownCommand.h"

void WeaponCycleDownCommand::execute(Entity& t_entity, EventManager& t_eventManager, float t_dt)
{
	t_eventManager.emitEvent(WeaponCycle{ false , t_entity});
}
