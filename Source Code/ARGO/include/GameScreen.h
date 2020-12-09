#pragma once
#include "Utilities.h"
#include "EventManager.h"
#include "HealthSystem.h"
#include "PhysicsSystem.h"
#include "InputSystem.h "
#include "RenderSystem.h"
#include "AiSystem.h"
#include "CollisionSystem.h"
#include "CommandSystem.h"
#include "ProjectileManager.h"
#include "EnemyManager.h"
#include "ParticleSystem.h"
#include "LevelManager.h"
#include "PlayerFactory.h"
#include "PickUpManager.h"
#include "EnemyFactory.h"
#include "HUDManager.h"
#include "ParticleManager.h"
#include "BloodManager.h"
#include "LightManager.h"
#include <WeaponSystem.h>

struct levelData
{
	glm::vec2 goalPos;
	float goalChargeTime;
};

enum class GoalState
{
	Inactive,
	Charging,
	Charged
};

class GameScreen
{
public:

	GameScreen(SDL_Renderer* t_renderer, EventManager& t_eventManager, Controller t_controller[Utilities::S_MAX_PLAYERS], CommandSystem& t_commandSystem, InputSystem& t_input, RenderSystem& t_renderSystem);
	~GameScreen();

	void update(float t_deltaTime);
	void processEvents(SDL_Event* t_event);
	void render(SDL_Renderer* t_renderer);
	void reset(SDL_Renderer* t_renderer, Controller t_controller[Utilities::S_MAX_PLAYERS], ButtonCommandMap t_controllerButtonMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS]);
	void initialise(SDL_Renderer* t_renderer, ButtonCommandMap t_controllerButtonMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS], Controller t_controller[Utilities::S_MAX_PLAYERS], bool t_isOnline = false);
 
private:

	void createPlayer(Entity& t_player, int t_index, SDL_Renderer* t_renderer);
	void createGoal();
	void createPopups(SDL_Renderer* t_renderer);
	void setUpLevel();
	void createLevel1();
	void createLevel2();
	void createLevel3();
	void setupGoal();
	void updateGoalText();
	void newLevel();
	void activateGoal(const GoalHit& t_event);
	void preRender();
	void updateGoal(float t_deltaTime);
	void updatePlayers(float t_deltaTime);
	void updateEntities(float t_deltaTime);
	void updateProjectiles(float t_deltaTime);
	void updateLevelManager();
	void setControllerButtonMap(ButtonCommandMap t_controllerMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS]);
	void gameOver(const GameOver& t_event);
	void updatePlayerColour(const UpdatePlayerColour& t_event);
	void cycleWeapons(const WeaponCycle& t_event);

	Entity m_gameLosePopup;
	Entity m_gameWinPopup;


	EventManager& m_eventManager;
	SDL_Renderer* m_renderer;

	Controller m_controllers[Utilities::S_MAX_PLAYERS];
	Entity m_players[Utilities::S_MAX_PLAYERS];
	Entity m_goal;
	float m_goalCurrentCharge;
	bool m_goalIsCharging;
	GoalState m_goalState;
	levelData m_levelData[3];
	int m_currentLevel;


	// Systems
	HealthSystem m_healthSystem;
	PhysicsSystem m_transformSystem;
	AiSystem m_aiSystem;
	CollisionSystem m_collisionSystem;
	ParticleSystem m_particleSystem;
	WeaponSystem m_weaponSystem;

	CommandSystem& m_commandSystem;
	InputSystem& m_inputSystem;
	RenderSystem& m_renderSystem;

	ProjectileManager m_projectileManager;
	LevelManager m_levelManager;
	EnemyManager m_enemyManager;
	HUDManager m_hudManager;
	BloodManager m_bloodManager;
	LightManager m_lightManager;
 
	PlayerFactory m_playerFactory;
	PickUpManager m_pickUpManager;
	ParticleManager m_particleManager;

	ButtonCommandMap m_controllerButtonMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS];

	// bool to see if game is being played online or not
	bool m_isOnline;

	bool m_gameOver;
	bool m_isGameLost;

	const int MAX_LEVEL = 2;
};
