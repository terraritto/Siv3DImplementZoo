#include "CircleComponent.h"

CircleComponent::CircleComponent(std::weak_ptr<Actor> owner)
	: Component(owner)
{
}

void CircleComponent::Update(float deltaTime)
{
	auto pOwner = m_owner.lock();
	m_circle.setPos(pOwner->GetPosition());
}

void CircleComponent::SetRadius(float radius)
{
	m_circle.setR(m_owner.lock()->GetScale() * radius);
}

double CircleComponent::GetRadius() const
{
    return m_circle.r;
}

Vec2 CircleComponent::GetCenter() const
{
	return m_circle.center;
}

bool CircleComponent::Intersect(std::weak_ptr<CircleComponent> circle)
{
	if (auto pCircle = circle.lock())
	{
		return m_circle.intersects(pCircle->GetCirle());
	}

	return false;
}
