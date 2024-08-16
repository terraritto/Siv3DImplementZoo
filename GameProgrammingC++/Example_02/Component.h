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

	[[nodiscard]]
	int getUpdateOrder() const { return m_updateOrder; }

protected:
	std::weak_ptr<Actor> m_owner;
	int m_updateOrder;
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
