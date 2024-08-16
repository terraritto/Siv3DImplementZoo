#pragma once
#include "../AIState.h"

class AIComponent;

class AIMove : public AIState
{
public:
	AIMove(std::weak_ptr<AIComponent> owner)
		: AIState(owner)
		, m_speed(100.0f)
	{ }

	void Update() override;
	void OnEnter() override;
	void OnExit() override;

	const String GetName() const override
	{
		return U"move";
	}

protected:
	float m_speed;
};
