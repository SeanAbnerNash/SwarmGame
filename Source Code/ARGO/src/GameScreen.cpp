#include "stdafx.h"
#include "GameScreen.h"


bool cleanUpEnemies(const Entity& t_entity)
{
	return !static_cast<HealthComponent*>(t_entity.getComponent(ComponentType::Health))->isAlive();
}

GameScreen::GameScreen(SDL_Renderer* t_renderer, EventManager& t_eventManager, Controller t_controllers[Utilities::S_MAX_PLAYERS], CommandSystem& t_commandSystem, InputSystem& t_input, RenderSystem& t_renderSystem) :
	m_eventManager{ t_eventManager },
	m_controllers{ *t_controllers },
	m_levelManager{ t_renderer, m_players, m_renderSystem, m_projectileManager, m_lightManager, t_eventManager },
	m_enemyManager{ t_renderer, Utilities::ENEMY_INITIAL_SPAWN_DELAY, t_eventManager, m_transformSystem, m_collisionSystem, m_healthSystem, m_aiSystem, m_renderSystem, m_levelManager },
	m_renderer{ t_renderer },
	m_transformSystem{ m_eventManager },
	m_projectileManager{ t_renderer, m_eventManager, m_renderSystem.getFocus(), m_transformSystem, m_collisionSystem },
	m_aiSystem{ m_players, m_enemyManager.getEnemies(), m_pickUpManager.getPickups(), m_goal, m_eventManager, m_levelManager },
	m_collisionSystem{ m_eventManager },
	m_weaponSystem{ m_projectileManager, m_eventManager },
	m_playerFactory(),
	m_pickUpManager(m_eventManager, m_collisionSystem),
	m_commandSystem{ t_commandSystem },
	m_inputSystem{ t_input },
	m_renderSystem{ t_renderSystem },
	m_hudManager(m_players, m_eventManager),
	m_particleManager(m_eventManager, m_particleSystem),
	m_bloodManager(m_particleSystem, m_players),
	m_lightManager(m_eventManager),
	m_currentLevel(0),
	m_gameOver(false),
	m_goalCurrentCharge(0),
	m_goalIsCharging(false),
	m_goalState(GoalState::Inactive)
{
	t_eventManager.subscribeToEvent<GoalHit>(std::bind(&GameScreen::activateGoal, this, std::placeholders::_1));

	m_levelData[0].goalPos = glm::vec2(51, 36) * (float)Utilities::TILE_SIZE;
	m_levelData[0].goalChargeTime = 1200;
	m_levelData[1].goalPos = glm::vec2(55, 10) * (float)Utilities::TILE_SIZE;
	m_levelData[1].goalChargeTime = 3000;
	m_levelData[2].goalPos = glm::vec2(30, 20) * (float)Utilities::TILE_SIZE;
	m_levelData[2].goalChargeTime = 6000;
}

GameScreen::~GameScreen()
{
}

void GameScreen::update(float t_deltaTime)
{
	updatePlayers(t_deltaTime);
	if (!m_gameOver)
	{
		m_transformSystem.handleFriction(t_deltaTime);
		updateGoal(t_deltaTime);
		updateLevelManager();
		updateEntities(t_deltaTime);
		updateProjectiles(t_deltaTime);
		m_collisionSystem.update(m_goal);
		m_collisionSystem.handleCollisions();
		m_pickUpManager.update(t_deltaTime);
		m_hudManager.update();
		m_particleManager.update(t_deltaTime);
		m_bloodManager.update(t_deltaTime);
		m_lightManager.update(t_deltaTime);
	}
}

void GameScreen::processEvents(SDL_Event* t_event)
{
	switch (t_event->type)
	{
	case SDL_KEYDOWN:
	{
		switch (t_event->key.keysym.sym)
		{
#ifdef _DEBUG
		case SDLK_HOME:
		{
			m_eventManager.emitEvent<ChangeScreen>(ChangeScreen{ MenuStates::MainMenu });
			break;
		}
		case SDLK_RETURN:
		{
			for (Entity& p : m_players)
			{
				(static_cast<HealthComponent*>(p.getComponent(ComponentType::Health))->setHealth(0));

			}
			break;
		}
		case SDLK_1:
		{
			m_eventManager.emitEvent<GameOver>(GameOver{ false});
			break;
		}
#endif // _DEBUG

		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

void GameScreen::render(SDL_Renderer* t_renderer)
{
	preRender();
	m_levelManager.render(t_renderer, &m_renderSystem);
	m_enemyManager.render(t_renderer);
	m_bloodManager.render(t_renderer, &m_renderSystem);
	for (Entity& player : m_players)
	{
		if (static_cast<HealthComponent*>(player.getComponent(ComponentType::Health))->isAlive())
		{
			m_renderSystem.render(t_renderer, player);
		}
	}
	m_renderSystem.render(t_renderer, m_goal);
	m_projectileManager.render(t_renderer, &m_renderSystem);
	m_pickUpManager.render(t_renderer, &m_renderSystem);
	m_particleManager.render(t_renderer, &m_renderSystem);
	m_levelManager.renderLight(t_renderer, &m_renderSystem);
	m_hudManager.render(t_renderer, &m_renderSystem);

	if (m_gameOver)
	{
		if (m_isGameLost)
		{
			m_renderSystem.render(t_renderer, m_gameLosePopup);
		}
		else
		{
			m_renderSystem.render(t_renderer, m_gameWinPopup);
		}
	}
}

void GameScreen::createPlayer(Entity& t_player, int t_index, SDL_Renderer* t_renderer)
{
	bool humanControlledPlayer = true;
	if (!m_controllers[t_index].getSDLController())
	{
		humanControlledPlayer = false;
	}
	m_playerFactory.createPlayer(t_player, humanControlledPlayer, m_controllers[t_index], t_index, m_controllerButtonMaps);
}

void GameScreen::createGoal()
{
	m_goal.addComponent(new TransformComponent());
	m_goal.addComponent(new ColliderCircleComponent(32));
	m_goal.addComponent(new TagComponent(Tag::Goal));
	m_goal.addComponent(new VisualComponent("EscapePod.png", m_renderer));
	m_goal.addComponent(new TextComponent("ordinary.ttf", m_renderer, 24, false, " "));
}

void GameScreen::createPopups(SDL_Renderer* t_renderer)
{
	m_gameLosePopup.addComponent(new VisualComponent("GameLost_Pop_Up.png", t_renderer));
	VisualComponent* visualComp = static_cast<VisualComponent*>(m_gameLosePopup.getComponent(ComponentType::Visual));
	visualComp->setStaticPosition(true);
	m_gameLosePopup.addComponent(new TransformComponent(glm::vec2(Utilities::SCREEN_WIDTH / 2.0f - visualComp->getWidth() / 2.0f, Utilities::SCREEN_HEIGHT / 2.0f - visualComp->getHeight() / 2.0f)));
	static_cast<TransformComponent*>(m_gameLosePopup.getComponent(ComponentType::Transform))->setAlwaysOnScreen(true);

	m_gameWinPopup.addComponent(new VisualComponent("GameWon_Pop_Up.png", t_renderer));
	visualComp = static_cast<VisualComponent*>(m_gameWinPopup.getComponent(ComponentType::Visual));
	visualComp->setStaticPosition(true);
	m_gameWinPopup.addComponent(new TransformComponent(glm::vec2(Utilities::SCREEN_WIDTH / 2.0f - visualComp->getWidth() / 2.0f, Utilities::SCREEN_HEIGHT / 2.0f - visualComp->getHeight() / 2.0f)));
	static_cast<TransformComponent*>(m_gameWinPopup.getComponent(ComponentType::Transform))->setAlwaysOnScreen(true);
}

void GameScreen::setUpLevel()
{
	setupGoal();

	m_levelManager.setupLevel();

	switch (m_currentLevel)
	{
	case 0:
		createLevel1();
		break;
	case 1:
		createLevel2();
		break;
	case 2:
		createLevel3();
		break;
	default:
		break;
	}
}

void GameScreen::createLevel1()
{
	//Creates a room at a given position with a given width and height
	m_levelManager.createRoom(glm::vec2(1, 1), 12, 12);
	m_levelManager.createRoom(glm::vec2(13, 2), 2, 2);
	m_levelManager.createRoom(glm::vec2(15, 1), 5, 12);
	m_levelManager.createRoom(glm::vec2(3, 13), 2, 2);
	m_levelManager.createRoom(glm::vec2(1, 15), 5, 12);
	m_levelManager.createRoom(glm::vec2(6, 23), 2, 2);
	m_levelManager.createRoom(glm::vec2(8, 15), 20, 18);
	m_levelManager.createRoom(glm::vec2(17, 13), 2, 2);
	m_levelManager.createRoom(glm::vec2(17, 13), 2, 2);
	m_levelManager.createRoom(glm::vec2(1, 29), 5, 10);
	m_levelManager.createRoom(glm::vec2(6, 35), 8, 4);
	m_levelManager.createRoom(glm::vec2(10, 33), 2, 2);
	m_levelManager.createRoom(glm::vec2(22, 1), 20, 5);
	m_levelManager.createRoom(glm::vec2(24, 6), 2, 9);
	m_levelManager.createRoom(glm::vec2(30, 8), 6, 14);
	m_levelManager.createRoom(glm::vec2(32, 6), 2, 2);
	m_levelManager.createRoom(glm::vec2(28, 18), 2, 2);
	m_levelManager.createRoom(glm::vec2(24, 33), 2, 2);
	m_levelManager.createRoom(glm::vec2(16, 35), 14, 4);
	m_levelManager.createRoom(glm::vec2(28, 29), 2, 2);
	m_levelManager.createRoom(glm::vec2(30, 24), 4, 9);
	m_levelManager.createRoom(glm::vec2(34, 24), 5, 15);
	m_levelManager.createRoom(glm::vec2(30, 36), 4, 2);
	m_levelManager.createRoom(glm::vec2(42, 2), 2, 2);
	m_levelManager.createRoom(glm::vec2(44, 1), 14, 8);
	m_levelManager.createRoom(glm::vec2(36, 14), 2, 2);
	m_levelManager.createRoom(glm::vec2(38, 12), 10, 9);
	m_levelManager.createRoom(glm::vec2(50, 12), 8, 20);
	m_levelManager.createRoom(glm::vec2(41, 23), 9, 6);
	m_levelManager.createRoom(glm::vec2(39, 25), 2, 2);
	m_levelManager.createRoom(glm::vec2(48, 14), 2, 2);
	m_levelManager.createRoom(glm::vec2(54, 32), 2, 2);
	m_levelManager.createRoom(glm::vec2(41, 34), 17, 4);
	m_levelManager.createRoom(glm::vec2(41, 31), 7, 3);
}

void GameScreen::createLevel2()
{
	m_levelManager.createRoom(glm::vec2(1, 1), 20, 12);
	m_levelManager.createRoom(glm::vec2(21, 1), 25, 7);
	m_levelManager.createRoom(glm::vec2(49, 1), 10, 15);
	m_levelManager.createRoom(glm::vec2(1, 14), 8, 20);
	m_levelManager.createRoom(glm::vec2(9, 30), 8, 9);
	m_levelManager.createRoom(glm::vec2(10, 14), 20, 9);
	m_levelManager.createRoom(glm::vec2(10, 23), 10, 6);
	m_levelManager.createRoom(glm::vec2(21, 24), 8, 15);
	m_levelManager.createRoom(glm::vec2(30, 8), 8, 20);
	m_levelManager.createRoom(glm::vec2(29, 31), 30, 8);
	m_levelManager.createRoom(glm::vec2(38, 21), 15, 10);
	m_levelManager.createRoom(glm::vec2(40, 9), 5, 13);
}

void GameScreen::createLevel3()
{
	m_levelManager.createRoom(glm::vec2(1, 1), 58, 8);
	m_levelManager.createRoom(glm::vec2(51, 9), 8, 30);
	m_levelManager.createRoom(glm::vec2(1, 9), 8, 30);
	m_levelManager.createRoom(glm::vec2(1, 31), 58, 8);
	m_levelManager.createRoom(glm::vec2(28, 9), 4, 1);
	m_levelManager.createRoom(glm::vec2(28, 30), 4, 1);
	m_levelManager.createRoom(glm::vec2(9, 18), 1, 4);
	m_levelManager.createRoom(glm::vec2(50, 18), 1, 4);

	m_levelManager.createRoom(glm::vec2(10, 10), 40, 20);
}

void GameScreen::setupGoal()
{
	m_gameOver = false;
	m_goalCurrentCharge = 0;
	m_goalIsCharging = false;
	m_goalState = GoalState::Inactive;
	static_cast<TransformComponent*>(m_goal.getComponent(ComponentType::Transform))->setPos(m_levelData[m_currentLevel].goalPos);
}

void GameScreen::updateGoalText()
{
	TextComponent* comp = static_cast<TextComponent*>(m_goal.getComponent(ComponentType::Text));
	switch (m_goalState)
	{
	case GoalState::Inactive:
		comp->setText("INACTIVE");
		comp->setColor(255, 0, 0);
		break;
	case GoalState::Charging:
		comp->setText("CHARGING:" + std::to_string((int)(m_goalCurrentCharge / m_levelData[m_currentLevel].goalChargeTime * 100)) + "%");
		comp->setColor(255, 150, 50);
		break;
	case GoalState::Charged:
		comp->setText("CHARGED");
		comp->setColor(0, 255, 0);
		break;
	default:
		break;
	}
}

void GameScreen::newLevel()
{
	if (m_currentLevel > MAX_LEVEL)
	{
		m_eventManager.emitEvent<UpdateAchievement>(UpdateAchievement{ 0, 1 });
		m_eventManager.emitEvent<ChangeScreen>(ChangeScreen{ MenuStates::MainMenu });
	}
	else
	{
		m_gameOver = false;
		int playerCount = 0;
		for (Entity& player : m_players)
		{
			player.removeAllComponents();
			createPlayer(player, playerCount, m_renderer);
			playerCount++;
		}
		setUpLevel();
		m_projectileManager.reset();
		m_enemyManager.killAll();
		m_hudManager.reset();
		m_pickUpManager.reset();
	}
}

void GameScreen::activateGoal(const GoalHit& t_event)
{
	if (m_goalState == GoalState::Inactive)
	{
		m_enemyManager.setDifficulty(666);
		m_goalState = GoalState::Charging;
	}
	if (m_goalState == GoalState::Charged)
	{
		m_currentLevel++;
		m_eventManager.emitEvent<UpdateAchievement>(UpdateAchievement{ 0, 1 });
		newLevel();
	}
}

void GameScreen::updatePlayers(float t_deltaTime)
{
	if (!static_cast<HealthComponent*>(m_players[0].getComponent(ComponentType::Health))->isAlive() &&
		!static_cast<HealthComponent*>(m_players[1].getComponent(ComponentType::Health))->isAlive() &&
		!static_cast<HealthComponent*>(m_players[2].getComponent(ComponentType::Health))->isAlive() &&
		!static_cast<HealthComponent*>(m_players[3].getComponent(ComponentType::Health))->isAlive())
	{
		m_eventManager.emitEvent<GameOver>(GameOver{true });
	}
	for (Entity& player : m_players)
	{
		m_inputSystem.update(player);
		m_commandSystem.update(player, m_eventManager, t_deltaTime);
		if (static_cast<HealthComponent*>(player.getComponent(ComponentType::Health))->isAlive())
		{
			static_cast<FSMComponent*>(player.getComponent(ComponentType::FSM))->getFSM().setMoved(false);
			m_aiSystem.update(player, t_deltaTime);
			m_healthSystem.update(player, t_deltaTime);
			m_transformSystem.update(player, t_deltaTime);
			m_collisionSystem.update(player);
			m_particleSystem.update(player, t_deltaTime);
			m_weaponSystem.update(player, t_deltaTime);
			static_cast<FSMComponent*>(player.getComponent(ComponentType::FSM))->getFSM().update(t_deltaTime);
		}
	}
}

void GameScreen::updateEntities(float t_deltaTime)
{
	m_enemyManager.update(t_deltaTime);
}

void GameScreen::updateProjectiles(float t_deltaTime)
{
	m_projectileManager.update(t_deltaTime);
}

void GameScreen::updateLevelManager()
{
	m_levelManager.checkWallDamage();
	m_levelManager.update(&m_collisionSystem);
}

void GameScreen::setControllerButtonMap(ButtonCommandMap t_controllerMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS])
{
	for (int index = 0; index < Utilities::S_MAX_PLAYERS; index++)
	{
		m_controllerButtonMaps[static_cast<int>(ButtonState::Pressed)][index] = t_controllerMaps[static_cast<int>(ButtonState::Pressed)][index];
		m_controllerButtonMaps[static_cast<int>(ButtonState::Held)][index] = t_controllerMaps[static_cast<int>(ButtonState::Held)][index];
		m_controllerButtonMaps[static_cast<int>(ButtonState::Released)][index] = t_controllerMaps[static_cast<int>(ButtonState::Released)][index];
	}
}

void GameScreen::gameOver(const GameOver& t_event)
{
	m_gameOver = true;
	m_isGameLost = t_event.isGameLost;
	using ButtonCommandPair = std::pair<ButtonType, Command*>;
	for (int index = 0; index < Utilities::S_MAX_PLAYERS; index++)
	{
		if ((static_cast<InputComponent*>(m_players[index].getComponent(ComponentType::Input))))
		{
			m_controllerButtonMaps[static_cast<int>(ButtonState::Pressed)][index] =
			{
				ButtonCommandPair(ButtonType::B, new GoToMainMenuCommand()),
				ButtonCommandPair(ButtonType::Back, new GoToMainMenuCommand())
			};
			m_controllerButtonMaps[static_cast<int>(ButtonState::Held)][index] = ButtonCommandMap();
			m_controllerButtonMaps[static_cast<int>(ButtonState::Released)][index] = ButtonCommandMap();

			(static_cast<InputComponent*>(m_players[index].getComponent(ComponentType::Input)))->setButtonMap(ButtonState::Pressed, m_controllerButtonMaps[static_cast<int>(ButtonState::Pressed)][index]);
			(static_cast<InputComponent*>(m_players[index].getComponent(ComponentType::Input)))->setButtonMap(ButtonState::Held, m_controllerButtonMaps[static_cast<int>(ButtonState::Held)][index]);
			(static_cast<InputComponent*>(m_players[index].getComponent(ComponentType::Input)))->setButtonMap(ButtonState::Released, m_controllerButtonMaps[static_cast<int>(ButtonState::Released)][index]);
		}
	}
}

void GameScreen::updatePlayerColour(const UpdatePlayerColour& t_event)
{
	VisualComponent* visComp = static_cast<VisualComponent*>(m_players[t_event.playerIndex].getComponent(ComponentType::Visual));
	visComp->setColor(t_event.colour.x, t_event.colour.y, t_event.colour.z);
}

void GameScreen::cycleWeapons(const WeaponCycle& t_event)
{
	static_cast<WeaponComponent*>(t_event.player.getComponent(ComponentType::Weapon))->cycleCurrentWeapon(t_event.isCycleUp);
}

void GameScreen::preRender()
{
	// Setting the focus point for the camera.
	glm::vec2 focusPoint = glm::vec2(0.0f, 0.0f);
	float numberOfPoints = 0;
	for (Entity& player : m_players)
	{
		TransformComponent* transformComp = static_cast<TransformComponent*>(player.getComponent(ComponentType::Transform));
		InputComponent* inputComp = static_cast<InputComponent*>(player.getComponent(ComponentType::Input));
		HealthComponent* healthComp = static_cast<HealthComponent*>(player.getComponent(ComponentType::Health));
		if (transformComp && inputComp && healthComp->isAlive())
		{
			numberOfPoints++;
			focusPoint += transformComp->getPos();
		}
	}
	if (numberOfPoints == 0)
	{
		focusPoint = static_cast<TransformComponent*>(m_players[0].getComponent(ComponentType::Transform))->getPos();
		numberOfPoints = 1;
	}
	m_renderSystem.setFocus(focusPoint / numberOfPoints);
}

void GameScreen::updateGoal(float t_deltaTime)
{
	if (m_goalState == GoalState::Charging)
	{
		m_goalCurrentCharge += t_deltaTime;
		if (m_goalCurrentCharge >= m_levelData[m_currentLevel].goalChargeTime)
		{
			m_goalState = GoalState::Charged;
		}
	}
	updateGoalText();
}

void GameScreen::reset(SDL_Renderer* t_renderer, Controller t_controller[Utilities::S_MAX_PLAYERS], ButtonCommandMap t_controllerButtonMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS])
{
	m_gameOver = false;
	m_currentLevel = 0;
	setControllerButtonMap(t_controllerButtonMaps);
	for (int index = 0; index < Utilities::S_MAX_PLAYERS; index++)
	{
		m_controllers[index] = t_controller[index];
	}
	int playerCount = 0;
	for (Entity& player : m_players)
	{
		player.removeAllComponents();
		createPlayer(player, playerCount, t_renderer);
		playerCount++;
	}
	setUpLevel();
	m_projectileManager.reset();
	m_enemyManager.killAll();
	m_hudManager.reset();
	m_pickUpManager.reset();
}

void GameScreen::initialise(SDL_Renderer* t_renderer, ButtonCommandMap t_controllerButtonMaps[Utilities::NUMBER_OF_CONTROLLER_MAPS][Utilities::S_MAX_PLAYERS], Controller t_controller[Utilities::S_MAX_PLAYERS], bool t_isOnline)
{
	m_gameOver = false;
	createPopups(t_renderer);
	m_isOnline = t_isOnline;
	setControllerButtonMap(t_controllerButtonMaps);
	for (int index = 0; index < Utilities::S_MAX_PLAYERS; index++)
	{
		m_controllers[index] = t_controller[index];
	}
	int playerCount = 0;
	m_playerFactory.initialise(t_renderer);
	for (Entity& player : m_players)
	{
		createPlayer(player, playerCount, t_renderer);
		playerCount++;
	}
	createGoal();
	m_enemyManager.init();
	setUpLevel();
	m_projectileManager.init();
	m_pickUpManager.init(m_renderer);
	m_hudManager.init(t_renderer);
	m_eventManager.subscribeToEvent<GameOver>(std::bind(&GameScreen::gameOver, this, std::placeholders::_1));
	m_eventManager.subscribeToEvent<UpdatePlayerColour>(std::bind(&GameScreen::updatePlayerColour, this, std::placeholders::_1));
	m_eventManager.subscribeToEvent<WeaponCycle>(std::bind(&GameScreen::cycleWeapons, this, std::placeholders::_1));


}
