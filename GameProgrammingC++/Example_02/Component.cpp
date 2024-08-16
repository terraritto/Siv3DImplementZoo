#include "Component.h"
#include "Actor.h"

Component::Component(std::weak_ptr<Actor> owner, int updateOrder)
	: m_owner(owner)
	, m_updateOrder(updateOrder)
{
}

Component::Component(Component& component)
{
	this->m_owner = component.m_owner;
	this->m_updateOrder = component.m_updateOrder;
}

Component::~Component()
{
}

void Component::Update(float deltaTime)
{
}
