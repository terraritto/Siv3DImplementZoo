﻿#include "MoveComponent.h"
#include "Util.h"

MoveComponent::MoveComponent(std::weak_ptr<Actor> owner, int updateOrder)
	: Component(owner)
	, m_angularSpeed(0.0f)
	, m_forwardSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	Size sceneSize = Window::GetState().virtualSize;

	auto pOwner = m_owner.lock();
	if (!pOwner)
	{
		return;
	}

	if (!NearZero(m_angularSpeed))
	{
		float rotation = pOwner->GetRotation();
		rotation += m_angularSpeed * deltaTime;
		pOwner->SetRotation(rotation);
	}

	if (!NearZero(m_forwardSpeed))
	{
		Vector2D<float> position = pOwner->GetPosition();
		position += pOwner->GetForward() * m_forwardSpeed * deltaTime;

		if (position.x < 0.0f) { position.x = sceneSize.x; }
		else if (position.x > sceneSize.x) { position.x = 0.0f; }

		if (position.y < 0.0f) { position.y = sceneSize.y; }
		else if (position.y > sceneSize.y) { position.y = 0.0f; }

		pOwner->SetPosition(position);
	}
}