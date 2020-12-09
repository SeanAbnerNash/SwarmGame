#include "stdafx.h"
#include "AiSystem.h"

AiSystem::AiSystem(Entity(&t_players)[Utilities::S_MAX_PLAYERS], Entity(&t_enemies)[Utilities::ENEMY_POOL_SIZE], Entity(&t_pickups)[Utilities::PICKUP_POOL_SIZE], Entity& t_goal, EventManager& t_eventManager, LevelManager& t_levelManager) :
	m_players(t_players),
	m_enemies(t_enemies),
	m_pickups(t_pickups),
	m_goal(t_goal),
	m_eventManager(t_eventManager),
	m_levelManager(t_levelManager),
	m_currentWaypoint(glm::linearRand(0, 4))
{
	m_behaviourTree.addChild(new RetreatBehaviour(&m_botEnemyData, m_levelManager));
	m_behaviourTree.addChild(new HoldBehaviour(&m_botEnemyData, m_levelManager));
	m_behaviourTree.addChild(new GetAmmoBehaviour(&m_botPickupData, m_levelManager));
	m_behaviourTree.addChild(new GetHealthBehaviour(&m_botHealthPickupData, m_levelManager));
	m_behaviourTree.addChild(new MoveToGoalBehaviour(&m_botGoalData, m_levelManager));
	m_behaviourTree.addChild(new MoveToLeaderBehaviour(&m_botLeaderData, m_levelManager));

	m_waypoints[1].destination = glm::vec2(Utilities::TILE_SIZE * 55, Utilities::TILE_SIZE * 3);
	m_waypoints[2].destination = glm::vec2(Utilities::TILE_SIZE * 3, Utilities::TILE_SIZE * 37);
	m_waypoints[3].destination = glm::vec2(Utilities::TILE_SIZE * 20, Utilities::TILE_SIZE * 37);
	m_waypoints[4].destination = glm::vec2(Utilities::TILE_SIZE * 20, Utilities::TILE_SIZE * 20);
}

AiSystem::~AiSystem()
{
	BaseSystem::~BaseSystem();
}

void AiSystem::update(Entity& t_entity)
{
}

void AiSystem::update(Entity& t_entity, float t_dt)
{
	TransformComponent* posComp = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform));
	AiComponent* aiComp = static_cast<AiComponent*>(t_entity.getComponent(ComponentType::Ai));
	ForceComponent* forceComp = static_cast<ForceComponent*>(t_entity.getComponent(ComponentType::Force));

	m_waypoints[0].destination = static_cast<TransformComponent*>(m_goal.getComponent(ComponentType::Transform))->getPos();
	//make sure that entity is not missing crucial components
	if (posComp && aiComp && forceComp)
	{
		switch (aiComp->getType())
		{
		case AITypes::eMelee:
			meleeAI(posComp, aiComp, forceComp, t_dt);
			break;
		case AITypes::eRanged:
			rangedAI(posComp, aiComp, forceComp, t_dt);
			break;
		case AITypes::ePlayerBot:
			playerAI(t_entity, t_dt);
			break;
		case AITypes::eWaller:
		{
			HealthComponent* healthComp = static_cast<HealthComponent*>(t_entity.getComponent(ComponentType::Health));
			wallerAi(posComp, aiComp, forceComp, healthComp, t_dt);
			break;
		}
		default:
			break;
		}
	}
}

void AiSystem::meleeAI(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, float t_dt)
{
	t_aiComponent->setState(AIStates::eWander);
	Entity* tile = m_levelManager.findAtPosition(t_posComp->getPos());
	glm::vec2 seekPosition = glm::vec2(0, 0);
	if (tile)
	{
		FlowFieldComponent* flowFieldComp = static_cast<FlowFieldComponent*>(tile->getComponent(ComponentType::FlowField));
		if (flowFieldComp && flowFieldComp->getWeight() < Utilities::MAX_FLOW_FIELD_WEIGHT)
		{
			if (flowFieldComp->getWeight() == 0)
			{
				for (auto& player : m_players)
				{
					HealthComponent* healthComp = static_cast<HealthComponent*>(player.getComponent(ComponentType::Health));
					if (healthComp && healthComp->isAlive())
					{
						TransformComponent* transComp = static_cast<TransformComponent*>(player.getComponent(ComponentType::Transform));
						if (glm::distance2(t_posComp->getPos(), transComp->getPos()) < Utilities::TILE_SIZE * Utilities::TILE_SIZE)
						{
							seekPosition = transComp->getPos();
							t_aiComponent->setState(AIStates::eSeek);
						}
					}
				}
			}
			else if (flowFieldComp->getClosestNeightbour())
			{
				TransformComponent* transComp = static_cast<TransformComponent*>(flowFieldComp->getClosestNeightbour()->getComponent(ComponentType::Transform));
				if (transComp)
				{
					seekPosition = transComp->getPos() + glm::vec2(Utilities::TILE_SIZE / 2, Utilities::TILE_SIZE / 2);
					t_aiComponent->setState(AIStates::eSeek);
				}
			}
		}
	}

	//The Only Possible States Available to Melee Enemies. Use to limit Behaviours
	switch (t_aiComponent->getStates())
	{
	case AIStates::eSleeping:
		sleep(t_posComp, t_aiComponent, t_forceComponent, t_dt);
		break;
	case AIStates::eWander:
		wander(t_posComp, t_aiComponent, t_forceComponent, t_dt);
		break;
	case AIStates::eSeek:
		seek(t_posComp, t_aiComponent, t_forceComponent, seekPosition, t_dt);
		break;
	}
}

void AiSystem::rangedAI(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, float t_dt)
{
	//The Only Possible States Available to Ranged Enemies. Use to limit Behaviours
	switch (t_aiComponent->getStates())
	{
	case AIStates::eSleeping:
		sleep(t_posComp, t_aiComponent, t_forceComponent, t_dt);
		break;
	case AIStates::eWander:
		wander(t_posComp, t_aiComponent, t_forceComponent, t_dt);
		break;
	}
}

void AiSystem::wallerAi(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, HealthComponent* t_healthComponent, float t_dt)
{
	switch (t_aiComponent->getStates())
	{
	case AIStates::eSeek:
	{
		if (t_aiComponent->getSeekPos() == glm::vec2(-1, -1))
		{
			glm::vec2 pos = getRandomFloorTile() + glm::vec2(Utilities::TILE_SIZE, Utilities::TILE_SIZE) / 2.0f;
			glm::vec2 badPos = glm::vec2(-1, -1) + glm::vec2(Utilities::TILE_SIZE, Utilities::TILE_SIZE) / 2.0f;
			if (pos != badPos)
			{
				t_aiComponent->setSeekPos(pos);
			}
			else
			{
				t_healthComponent->setHealth(0);
			}
		}
		wallerSeek(t_posComp, t_aiComponent, t_forceComponent, t_aiComponent->getSeekPos(), t_healthComponent, t_dt);
		break;
	}
	default:
		break;
	}
}

void AiSystem::seek(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, glm::vec2 t_destination, float t_dt)
{
	glm::vec2 direction = glm::normalize(t_destination - t_posComp->getPos());
	t_forceComponent->addForce(direction * t_dt);
	t_posComp->setRotation(glm::degrees(atan2(direction.y, direction.x)));
}

void AiSystem::wallerSeek(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, glm::vec2 t_destination, HealthComponent* t_healthComponent, float t_dt)
{
	if (glm::distance2(t_destination + glm::vec2(Utilities::TILE_SIZE / 2.0f, Utilities::TILE_SIZE / 2.0f), t_posComp->getPos()) < std::pow(Utilities::TILE_SIZE, 2))
	{
		Entity* tile = m_levelManager.findAtPosition(t_posComp->getPos());
		if (tile)
		{
			m_levelManager.setToWall(*tile);
			t_aiComponent->setSeekPos(glm::vec2(-1, -1));
			t_healthComponent->setHealth(0);
		}
	}
	else
	{
		glm::vec2 direction = glm::normalize(t_destination - t_posComp->getPos());
		t_forceComponent->addForce(direction * t_dt);
		t_posComp->setRotation(glm::degrees(atan2(direction.y, direction.x)));
	}
}

void AiSystem::playerAI(Entity& t_entity, float t_dt)
{
	playerMovementDecision(t_entity, t_dt);
	playerShootingDecision(t_entity);
}

void AiSystem::playerMovementDecision(Entity& t_entity, float t_dt)
{
	//set up data
	glm::vec2 botPos = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform))->getPos();
	setLeader();
	setCurrentWaypoint();
	m_botEnemyData.nearbyEnemies = 0;
	m_botPickupData.entity = nullptr;
	m_botHealthPickupData.entity = nullptr;
	setEnemyData(botPos);
	setClosestLeaderData(botPos);
	setClosestPickupData(botPos);
	setGoalData(botPos);
	//query behaviour tree
	m_behaviourTree.run(t_entity, t_dt);
}

void AiSystem::playerShootingDecision(Entity& t_entity)
{
	if (m_botEnemyData.distance < CAN_SEE_ENEMY_DISTANCE * CAN_SEE_ENEMY_DISTANCE)
	{
		TransformComponent* playerTransform = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform));
		glm::vec2 enemyPos = static_cast<TransformComponent*>(m_botEnemyData.enemy->getComponent(ComponentType::Transform))->getPos();
		Weapon weapon = static_cast<WeaponComponent*>(t_entity.getComponent(ComponentType::Weapon))->getCurrent();
		Controller temp;
		glm::vec2 fireDirection = glm::normalize(enemyPos - playerTransform->getPos());
		playerTransform->setRotation(glm::degrees(atan2(fireDirection.y, fireDirection.x)));
		m_eventManager.emitEvent(CreateBulletEvent{ t_entity, fireDirection , 32, weapon , temp });
	}
}
void AiSystem::setEnemyData(glm::vec2 t_botPosition)
{
	m_botEnemyData.distance = CAN_SEE_ENEMY_DISTANCE * CAN_SEE_ENEMY_DISTANCE;
	for (auto& enemy : m_enemies)
	{
		if (enemy.getComponent(ComponentType::ColliderCircle))
		{
			TransformComponent* transCompEnemy = static_cast<TransformComponent*>(enemy.getComponent(ComponentType::Transform));
			HealthComponent* healthCompEnemy = static_cast<HealthComponent*>(enemy.getComponent(ComponentType::Health));
			if (transCompEnemy && healthCompEnemy && healthCompEnemy->isAlive())
			{
				float newDistance = glm::distance2(t_botPosition, transCompEnemy->getPos());
				if (newDistance < m_botEnemyData.distance)
				{
					m_botEnemyData.nearbyEnemies++;
					m_botEnemyData.enemy = &enemy;
					m_botEnemyData.distance = newDistance;
				}
			}
		}
	}
}

void AiSystem::setClosestLeaderData(glm::vec2 t_botPosition)
{
	m_botLeaderData.entity = &m_players[0];
	TransformComponent* transCompPlayer = static_cast<TransformComponent*>(m_players[0].getComponent(ComponentType::Transform));
	m_botLeaderData.distance = glm::distance2(t_botPosition, transCompPlayer->getPos());
	for (auto& player : m_players) //if there are any players we instead make the goal one of the players
	{
		if (player.getComponent(ComponentType::Input))
		{
			transCompPlayer = static_cast<TransformComponent*>(player.getComponent(ComponentType::Transform));
			if (transCompPlayer)
			{
				float newDistance = glm::distance2(t_botPosition, transCompPlayer->getPos());
				if (newDistance < m_botLeaderData.distance)
				{
					m_botLeaderData.entity = &player;
					m_botLeaderData.distance = newDistance;
				}
			}
		}
	}
}

void AiSystem::setClosestPickupData(glm::vec2 t_botPosition)
{
	m_botHealthPickupData.distance = CAN_SEE_ENEMY_DISTANCE * CAN_SEE_ENEMY_DISTANCE;
	m_botPickupData.distance = CAN_SEE_ENEMY_DISTANCE * CAN_SEE_ENEMY_DISTANCE;
	for (auto& pickup : m_pickups)
	{
		if (pickup.getComponent(ComponentType::ColliderCircle))
		{
			TransformComponent* transComp = static_cast<TransformComponent*>(pickup.getComponent(ComponentType::Transform));
			HealthComponent* healthComp = static_cast<HealthComponent*>(pickup.getComponent(ComponentType::Health));
			PickUpComponent* pickupComp = static_cast<PickUpComponent*>(pickup.getComponent(ComponentType::PickUp));
			if (transComp && healthComp && pickupComp && healthComp->isAlive())
			{
				float newDistance = glm::distance2(t_botPosition, transComp->getPos());
				switch (pickupComp->getPickupType())
				{
				case PickupType::Health:
					if (true)
					{
						if (newDistance < m_botHealthPickupData.distance)
						{
							m_botHealthPickupData.distance = newDistance;
							m_botHealthPickupData.entity = &pickup;
						}
					}
					break;
				case PickupType::MachineGun:
				case PickupType::Grenade:
				case PickupType::Shotgun:
					if (newDistance < m_botPickupData.distance)
					{
						m_botPickupData.distance = newDistance;
						m_botPickupData.entity = &pickup;
					}
					break;
				default:
					break;
				}
			}
		}
	}
}

void AiSystem::setGoalData(glm::vec2 t_botPosition)
{
	m_botGoalData.destination = m_waypoints[m_currentWaypoint].destination;
}

void AiSystem::setLeader()
{
	bool playerFound = false;
	for (auto& player : m_players)
	{
		if (!static_cast<AiComponent*>(player.getComponent(ComponentType::Ai)))
		{
			playerFound = true;
		}
	}

	if (!playerFound)
	{
		for (auto& player : m_players)
		{
			AiComponent* aiComp = static_cast<AiComponent*>(player.getComponent(ComponentType::Ai));
			HealthComponent* hpComp = static_cast<HealthComponent*>(player.getComponent(ComponentType::Health));
			if (hpComp->isAlive())
			{
				aiComp->setIsLeaser(true);
				break;
			}
		}
	}
}

void AiSystem::setCurrentWaypoint()
{
	glm::vec2 leaderPos;
	for (auto& player : m_players)
	{
		AiComponent* aiComp = static_cast<AiComponent*>(player.getComponent(ComponentType::Ai));
		HealthComponent* hpComp = static_cast<HealthComponent*>(player.getComponent(ComponentType::Health));
		TransformComponent* transformComp = static_cast<TransformComponent*>(player.getComponent(ComponentType::Transform));
		if (aiComp && hpComp->isAlive() && aiComp->getIsleader())
		{
			leaderPos = transformComp->getPos();
		}
	}

	//if the leader is close enough to its waypoint it decides to get a new waypoint.
	if (glm::distance2(leaderPos, m_waypoints[m_currentWaypoint].destination) < Utilities::TILE_SIZE * Utilities::TILE_SIZE)
	{
		bool success = false;

		while (!success)
		{
			int newWayPoint = glm::linearRand(0, 4);
			if (newWayPoint != m_currentWaypoint)
			{
				m_currentWaypoint = newWayPoint;
				success = true;
			}
		}
	}
}

void AiSystem::wander(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, float t_dt)
{
	//Gives a number between 1 and 20, -10 change the range to -9 and 10 then divides it by 10 to give a range of -.9 and 1.
	float tempAdjuster = (((rand() % 20 + 1)));
	tempAdjuster = (tempAdjuster - 10) / 10;

	//Randomly changes current angle by the max which is scaled by the random scalar
	t_posComp->setRotation(t_posComp->getRotation() + (t_aiComponent->getMaxRotation() * tempAdjuster));
	//Generates a unit vector in the given angle.
	glm::vec2 tempVelocity = glm::vec2(glm::cos(t_posComp->getRotation() * M_PI / 180), glm::sin(t_posComp->getRotation() * M_PI / 180));
	//Scales the Unit Vector by the length of the max speed.
	tempVelocity *= glm::length(t_aiComponent->getMaxSpeed());
	//Updates Position
	t_forceComponent->addForce(tempVelocity * t_dt);
	//t_posComp->setPos(t_posComp->getPos() + tempVelocity);
}

void AiSystem::sleep(TransformComponent* t_posComp, AiComponent* t_aiComponent, ForceComponent* t_forceComponent, float t_dt)
{
	//Nothing will add code to awake unit if a target (i.e player) comes within range once discussions are had.
}

glm::vec2 AiSystem::getRandomFloorTile()
{
	Entity* tile = m_levelManager.findAtPosition(glm::vec2(glm::linearRand(0, Utilities::LEVEL_TILE_WIDTH * Utilities::TILE_SIZE), glm::linearRand(0, Utilities::LEVEL_TILE_HEIGHT * Utilities::TILE_SIZE)));

	if (tile && nullptr != tile->getComponent(ComponentType::ColliderAABB))
	{
		//tile = m_levelManager.findAtPosition(glm::vec2(glm::linearRand(0, Utilities::LEVEL_TILE_WIDTH * Utilities::TILE_SIZE), glm::linearRand(0, Utilities::LEVEL_TILE_HEIGHT * Utilities::TILE_SIZE)));
		TileComponent* tileComp = static_cast<TileComponent*>(tile->getComponent(ComponentType::Tile));
		if (tileComp->getNeighbours()->bottom && tileComp->getNeighbours()->bottom->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->bottom;
		}
		else if (tileComp->getNeighbours()->bottomLeft && !tileComp->getNeighbours()->bottomLeft->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->bottomLeft;
		}
		else if (tileComp->getNeighbours()->bottomRight && !tileComp->getNeighbours()->bottomRight->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->bottomRight;
		}
		else if (tileComp->getNeighbours()->left && !tileComp->getNeighbours()->left->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->left;
		}
		else if (tileComp->getNeighbours()->right && !tileComp->getNeighbours()->right->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->right;
		}
		else if (tileComp->getNeighbours()->top && !tileComp->getNeighbours()->top->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->top;
		}
		else if (tileComp->getNeighbours()->topLeft && !tileComp->getNeighbours()->topLeft->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->topLeft;
		}
		else if (tileComp->getNeighbours()->topRight&& !tileComp->getNeighbours()->topRight->getComponent(ComponentType::ColliderAABB))
		{
			tile = tileComp->getNeighbours()->topRight;
		}
		else
		{
			return glm::vec2(-1, -1);
		}
	}
	else
	{
		return glm::vec2(-1, -1);
	}

	return static_cast<TransformComponent*>(tile->getComponent(ComponentType::Transform))->getPos();
}
