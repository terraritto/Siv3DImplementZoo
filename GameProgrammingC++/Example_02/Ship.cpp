#include "Ship.h"
#include "SpriteComponent.h"

Ship::Ship(Game* game)
	: Actor(game)
	, m_rightSpeed(0.0f)
	, m_downSpeed(0.0f)
{
}

void Ship::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	Vec2 pos = GetPosition();
	pos.x += m_rightSpeed * deltaTime;
	pos.y += m_downSpeed * deltaTime;

	pos.x = Math::Clamp(pos.x, 50.0, 750.0);
	pos.y = Math::Clamp(pos.y, 50.0, 550.0);

	SetPosition(pos);
}

void Ship::ProcessKeyboard()
{
	m_rightSpeed = 0.0f;
	m_downSpeed = 0.0f;

	if (KeyD.pressed())
	{
		m_rightSpeed += 250.0f;
	}

	if (KeyA.pressed())
	{
		m_rightSpeed -= 250.0f;
	}

	if (KeyS.pressed())
	{
		m_downSpeed += 300.0f;
	}

	if (KeyW.pressed())
	{
		m_downSpeed -= 300.0f;
	}
}
