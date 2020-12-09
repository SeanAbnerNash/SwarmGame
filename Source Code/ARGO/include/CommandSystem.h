#pragma once
#include "BaseSystem.h"
#include "CommandComponent.h"
#include "InputComponent.h"

class CommandSystem :
	public BaseSystem
{
public:
	CommandSystem();
	~CommandSystem();
	void update(Entity& t_entity, EventManager& t_eventManager, float t_dt);
	void update(Entity& t_entity) {}
};

