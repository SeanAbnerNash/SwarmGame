#pragma once
#include "Entity.h"
#include "BaseSystem.h"
#include "RenderSystem.h"
#include "SDL_image.h"
#include "HUDComponent.h"
#include "TransformComponent.h"
#include "ColourComponent.h"
#include "PrimitiveComponent.h"
#include "VisualComponent.h"
#include "HealthComponent.h"
#include "TextComponent.h"
#include "WeaponComponent.h"

struct HUDBlock
{
	Entity HUDVisualTexture;
	Entity HUDHealthBar;
	Entity HUDAmmoBar;
	Entity HUDHealthText;
	Entity HUDAmmoText;
	Entity HUDAvatarIcon;
	Entity HUDLayoutData;
	Entity HUDAmmoBox[4];
	Entity HUDInfAmmo;
	bool showInfAmmo = false;
	bool showAmmoType[4]{ false,false,false,false };
	glm::vec2 previousSize;
	int timeSinceHealthChanged;
};

class HUDManager
{
public:
	HUDManager(Entity(&t_players)[Utilities::S_MAX_PLAYERS], EventManager& t_eventManager);
	void init(SDL_Renderer* t_renderer);
	void update();
	void render(SDL_Renderer* t_renderer, RenderSystem* t_system);
	void setUpHUD(HUDBlock& t_hudBlock, int t_playerIndex);
	void swapToSkullAvatar(VisualComponent* t_visComp);
	void reset();
private:
	HUDBlock m_playerHUD[Utilities::S_MAX_PLAYERS];

	Entity(&m_players)[Utilities::S_MAX_PLAYERS];
	SDL_Renderer* m_renderer;
	const int DAMAGE_SHOW_TIME = 200;

	EventManager& m_eventManager;

};

