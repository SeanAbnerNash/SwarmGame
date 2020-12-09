#pragma once
#include "Entity.h"
#include "BaseSystem.h"
#include "SDL_image.h"
#include "EventManager.h"
#include "TransformComponent.h"
#include "HealthComponent.h"
#include "TimerComponent.h"


class LightManager
{
public:
	LightManager(EventManager& t_eventManager);
	void createLight(Entity& t_entity);
	void placeLightExplosion(const Explosion& t_event);
	void update(float t_dt);
	void nextAvailableLight();
	Entity(&getLights())[Utilities::LIGHT_MAX_POOL];
private:
	Entity m_lightEmitter[Utilities::LIGHT_MAX_POOL];
	int m_currentEmitter{ 0 };
};
