#include "Actor.h"
#include "Game.h"
#include "Component.h"

Actor::Actor(Game* game)
	: m_state(State::Active)
	, m_position(0.0f, 0.0f)
	, m_scale(1.0f)
	, m_rotation(0.0f)
{
}

Actor::~Actor()
{
	m_componentList.clear();
}

void Actor::Update(float deltaTime)
{
	if (m_state == State::Active)
	{
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto& comp : m_componentList)
	{
		comp->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::AddComponent(std::shared_ptr<Component> component)
{
	int myOrder = component->getUpdateOrder();
	auto iter = m_componentList.begin();
	for (; iter != m_componentList.end(); iter++)
	{
		if (myOrder < (*iter)->getUpdateOrder())
		{
			break;
		}
	}

	m_componentList.insert(iter, component);
}
