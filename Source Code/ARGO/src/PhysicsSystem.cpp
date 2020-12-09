#include "stdafx.h"
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(EventManager& t_eventManager) :
	m_frictionTimer(0),
	m_applyFriction(0)
{
	t_eventManager.subscribeToEvent<PhysicsMove>(std::bind(&PhysicsSystem::updateWithInput, this, std::placeholders::_1));
	t_eventManager.subscribeToEvent<PhysicsRotate>(std::bind(&PhysicsSystem::updateRotation, this, std::placeholders::_1));
}

PhysicsSystem::~PhysicsSystem()
{
	BaseSystem::~BaseSystem();
}

void PhysicsSystem::update(Entity& t_entity, float t_dt)
{
	TransformComponent* transformComp = static_cast<TransformComponent*>(t_entity.getComponent(ComponentType::Transform));
	if (transformComp)
	{
		ForceComponent* forceComp = static_cast<ForceComponent*>(t_entity.getComponent(ComponentType::Force));
		if (forceComp)
		{
			transformComp->addPos(forceComp->getForce() * t_dt);
			if (forceComp->getHasFriction())
			{
				for (int i = 0; i < m_applyFriction; i++)
				{
					forceComp->setForce(forceComp->getForce() * FRICTION_SCALAR);
				}
			}
		}
		handleRotation(transformComp);
	}
}

void PhysicsSystem::update(Entity& t_entity)
{
}

void PhysicsSystem::handleFriction(float t_dt)
{
	m_applyFriction = 0;
	m_frictionTimer += t_dt;
	while (m_frictionTimer > 1)
	{
		m_frictionTimer--;
		m_applyFriction++;
	}
}

void PhysicsSystem::updateWithInput(const PhysicsMove& t_event)
{
	ForceComponent* forceComp = static_cast<ForceComponent*>(t_event.entity.getComponent(ComponentType::Force));
	if (forceComp)
	{
		forceComp->addForce(t_event.velocity);
	}
	TransformComponent* transformComp = static_cast<TransformComponent*>(t_event.entity.getComponent(ComponentType::Transform));
	if (transformComp)
	{
		transformComp->setLeftRotation(glm::degrees(atan2f(t_event.velocity.y, t_event.velocity.x)));
	}
}

void PhysicsSystem::updateRotation(const PhysicsRotate& t_event)
{ 
	TransformComponent* transformComp = static_cast<TransformComponent*>(t_event.entity.getComponent(ComponentType::Transform));
	if (transformComp)
	{
 		transformComp->setRightRotation(glm::degrees(atan2f(t_event.rotation.y, t_event.rotation.x)));
	}
}

void PhysicsSystem::handleRotation(TransformComponent* t_transformComp)
{
	// if -1 rotation, it is not set
	if (t_transformComp->getRightRotation() != -1)
	{
		t_transformComp->setRotation(t_transformComp->getRightRotation());
	}
	else if (t_transformComp->getLeftRotation() != -1)
	{
		t_transformComp->setRotation(t_transformComp->getLeftRotation());
	}
	// setting to -1 to allow for check if they are not valid
	t_transformComp->setRightRotation(-1);
	t_transformComp->setLeftRotation(-1);
}
