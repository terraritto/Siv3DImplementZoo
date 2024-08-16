#pragma once
#include "Actor.h"

class Ship : public Actor
{
public:
	Ship(Game* game);

	// 更新関係
	virtual void UpdateActor(float deltaTime) override;
	void ProcessKeyboard();

	[[nodiscard]]
	float GetRightSpeed() const { return m_rightSpeed; }

	[[nodiscard]]
	float GetDownSpeed() const { return m_downSpeed; }

private:
	float m_rightSpeed;
	float m_downSpeed;
};
