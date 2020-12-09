#pragma once
#include "Entity.h"
#include "BaseSystem.h"
#include "RenderSystem.h"
#include "SDL_image.h"
#include "ParticleEmitterComponent.h"
#include "PrimitiveComponent.h"
#include "ColourComponent.h"
#include "EventManager.h"
#include "TransformComponent.h"
#include "ParticleSystem.h"
#include "HealthComponent.h"


class BloodManager
{
public:
	BloodManager(ParticleSystem& t_particleSystem, Entity(&t_players)[Utilities::S_MAX_PLAYERS]);
	void createBloodEmitter(Entity& t_entity);
	void update(float t_dt);
	void render(SDL_Renderer* t_renderer, RenderSystem* t_system);
private:
	Entity m_bloodEmitter[Utilities::S_MAX_PLAYERS];
	SDL_Renderer* m_renderer;
	ParticleSystem& m_particleSystem;
	Entity(&m_players)[Utilities::S_MAX_PLAYERS];
};
