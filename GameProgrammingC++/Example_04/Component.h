#pragma once
#include <memory>
#include "Actor.h"

class Component
{
public:
	Component(std::weak_ptr<Actor> owner, int updateOrder = 100);
	Component(Component& component);
	virtual ~Component();

	virtual void Update(float deltaTime);
	virtual void ProcessInput() {}

	[[nodiscard]]
	int getUpdateOrder() const { return m_updateOrder; }

	[[nodiscard]]
	std::weak_ptr<Actor> GetOwner() { return m_owner; }

	[[nodiscard]]
	std::weak_ptr<Game> GetGame() { return m_game; }

protected:
	std::weak_ptr<Actor> m_owner;
	int m_updateOrder;

	std::weak_ptr<Game> m_game;
};

template<class T, class... Types>
std::weak_ptr<Component> CreateComponent(std::weak_ptr<Actor> actor, Types... values)
{
	std::shared_ptr<Component> component;
	if (auto pActor = actor.lock())
	{
		component = std::make_shared<T>(actor, values...);
		pActor->AddComponent(component);
	}

	return component;
}

template<class T, class... Types>
std::weak_ptr<T> CreateCastComponent(std::weak_ptr<Actor> actor, Types... values)
{
	std::shared_ptr<T> component;
	if (auto pActor = actor.lock())
	{
		component = std::make_shared<T>(actor, values...);
		pActor->AddComponent(component);
	}

	return component;
}
