#include "InputComponent.h"

InputComponent::InputComponent(std::weak_ptr<Actor> owner)
	: MoveComponent(owner)
	, m_forwardKey(KeyW)
	, m_backKey(KeyS)
	, m_clockwiseKey(KeyD)
	, m_counterClockwiseKey(KeyA)
{
}

void InputComponent::ProcessInput()
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;

	auto AddSpeed = [](float& speed, float maxSpeed, s3d::Input key)
	{
		if (key.pressed())
		{
			speed += maxSpeed;
		}
	};

	AddSpeed(forwardSpeed, m_maxForwardSpeed, m_forwardKey);
	AddSpeed(forwardSpeed, -m_maxForwardSpeed, m_backKey);
	SetForwardSpeed(forwardSpeed);

	AddSpeed(angularSpeed, m_maxAngularSpeed, m_clockwiseKey);
	AddSpeed(angularSpeed, -m_maxAngularSpeed, m_counterClockwiseKey);
	SetAngularSpeed(angularSpeed);
}
