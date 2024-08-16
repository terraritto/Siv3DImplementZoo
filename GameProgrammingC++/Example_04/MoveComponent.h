#pragma once
#include "Component.h"

class MoveComponent : public Component
{
public:
	MoveComponent(std::weak_ptr<Actor> owner, int updateOrder = 100);

	virtual void Update(float deltaTime) override;

	void SetAngularSpeed(float speed) { m_angularSpeed = speed; }
	void SetForwardSpeed(float speed) { m_forwardSpeed = speed; }

	[[nodiscard]]
	float GetAngularSpeed() const { return m_angularSpeed; }

	[[nodiscard]]
	float GetForwardSpeed() const { return m_forwardSpeed; }

protected:
	float m_angularSpeed;
	float m_forwardSpeed;
};
