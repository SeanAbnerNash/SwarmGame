#include "stdafx.h"
#include "BloodManager.h"

BloodManager::BloodManager(ParticleSystem& t_particleSystem, Entity(&t_players)[Utilities::S_MAX_PLAYERS]):
	m_particleSystem(t_particleSystem),
	m_players(t_players)
{
	for (auto& emitter : m_bloodEmitter)
	{
		createBloodEmitter(emitter);
	}
}

void BloodManager::createBloodEmitter(Entity& t_entity)
{
	t_entity.addComponent(new TransformComponent(true));
	TransformComponent* transComp = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform));
	t_entity.addComponent(new ColourComponent());
	ColourComponent* colourComp = static_cast<ColourComponent*>(t_entity.getComponent(ComponentType::Colour));
	//Converting uVec4 into Colour structs
	Colour primaryColour{ Utilities::PRIMARY_PLAYER_BLOOD_COLOUR.x, Utilities::PRIMARY_PLAYER_BLOOD_COLOUR.y, Utilities::PRIMARY_PLAYER_BLOOD_COLOUR.z, Utilities::PRIMARY_PLAYER_BLOOD_COLOUR.a };
	Colour secondaryColour{ Utilities::SECONDARY_PLAYER_BLOOD_COLOUR.x, Utilities::SECONDARY_PLAYER_BLOOD_COLOUR.y, Utilities::SECONDARY_PLAYER_BLOOD_COLOUR.z, Utilities::SECONDARY_PLAYER_BLOOD_COLOUR.a };
	colourComp->setColor(primaryColour);
	colourComp->setSecondaryColour(secondaryColour);
	t_entity.addComponent(new PrimitiveComponent());
	PrimitiveComponent* primComp = static_cast<PrimitiveComponent*>(t_entity.getComponent(ComponentType::Primitive));
	t_entity.addComponent(new ParticleEmitterComponent(glm::vec2(200, 200), true, 90, 180.0f, 0.0f, 100, 7, 100, false, 0.1f));
	ParticleEmitterComponent* emitterComp = static_cast<ParticleEmitterComponent*>(t_entity.getComponent(ComponentType::ParticleEmitter));
	emitterComp->setOffset(25.0f);
}

void BloodManager::update(float t_dt)
{
	int count = 0;
	for (auto& emitter : m_bloodEmitter)
	{
		TransformComponent* transComp = static_cast<TransformComponent*>(emitter.getComponent(ComponentType::Transform));
		TransformComponent* playerTransComp = static_cast<TransformComponent*>(m_players[count].getComponent(ComponentType::Transform));
		HealthComponent* playerHealthComp = static_cast<HealthComponent*>(m_players[count].getComponent(ComponentType::Health));
		ParticleEmitterComponent* partComp = static_cast<ParticleEmitterComponent*>(emitter.getComponent(ComponentType::ParticleEmitter));
		if (playerHealthComp->getHealth() < playerHealthComp->getMaxHealth() / 2.0f)
		{
			partComp->setEmitting(true);
			transComp->setPos(playerTransComp->getPos());
			m_particleSystem.update(emitter, t_dt);
		}
		else
		{
			partComp->setEmitting(false);
		}
		count++;
	}
}

void BloodManager::render(SDL_Renderer* t_renderer, RenderSystem* t_system)
{
	int count = 0;
	for (auto& emitter : m_bloodEmitter)
	{
		HealthComponent* playerHealthComp = static_cast<HealthComponent*>(m_players[count].getComponent(ComponentType::Health));
		if (playerHealthComp->isAlive())
		{
			t_system->render(t_renderer, emitter);
		}
		count++;
	}
}
