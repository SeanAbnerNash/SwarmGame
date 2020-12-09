#include "stdafx.h"
#include "LightManager.h"

LightManager::LightManager(EventManager& t_eventManager)
{
	t_eventManager.subscribeToEvent<Explosion>(std::bind(&LightManager::placeLightExplosion, this, std::placeholders::_1));
	for (auto& emitter : m_lightEmitter)
	{
		createLight(emitter);
	}
}

void LightManager::createLight(Entity& t_entity)
{
	t_entity.addComponent(new TransformComponent(true));
	TransformComponent* transComp = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform));
	t_entity.addComponent(new HealthComponent(1,0));
	t_entity.addComponent(new TimerComponent(Utilities::LIGHT_MAX_POOL));
}

void LightManager::placeLightExplosion(const Explosion& t_event)
{
	TransformComponent* emitterTransComp = static_cast<TransformComponent*>(m_lightEmitter[m_currentEmitter].getComponent(ComponentType::Transform));
	emitterTransComp->setPos(t_event.position);
	HealthComponent* healthComp = static_cast<HealthComponent*>(m_lightEmitter[m_currentEmitter].getComponent(ComponentType::Health));
	healthComp->setHealth(1);
	TimerComponent* timeComp = static_cast<TimerComponent*>(m_lightEmitter[m_currentEmitter].getComponent(ComponentType::Timer));
	timeComp->reset();
	nextAvailableLight();
}

void LightManager::update(float t_dt)
{
	for (auto& emitter : m_lightEmitter)
	{
		TimerComponent* timeComp = static_cast<TimerComponent*>(emitter.getComponent(ComponentType::Timer));
		HealthComponent* healthComp = static_cast<HealthComponent*>(emitter.getComponent(ComponentType::Health));
		if (healthComp->isAlive())
		{
			if (!timeComp->tick(t_dt))
			{
				healthComp->setHealth(0);
			}
		}
	}
}

void LightManager::nextAvailableLight()
{
	m_currentEmitter++;
	if (m_currentEmitter == Utilities::LIGHT_MAX_POOL)
	{
		m_currentEmitter = 0;
	}
}

Entity(&LightManager::getLights())[Utilities::LIGHT_MAX_POOL]
{
	return m_lightEmitter;
}
