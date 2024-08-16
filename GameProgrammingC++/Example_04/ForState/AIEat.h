#pragma once
#include "../AIState.h"

class AIComponent;

class AIEat : public AIState
{
public:
	AIEat(std::weak_ptr<AIComponent> owner)
		: AIState(owner)
		, m_timer(0.5s)
	{ }

	void Update() override;
	void OnEnter() override;
	void OnExit() override;

	const String GetName() const override
	{
		return U"eat";
	}

protected:
	Timer m_timer;
};
