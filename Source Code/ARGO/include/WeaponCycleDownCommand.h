#pragma once
#include "Command.h"
class WeaponCycleDownCommand :
	public Command
{
public:
	virtual void execute(Entity& t_entity, EventManager& t_eventManager, float t_dt);
};

