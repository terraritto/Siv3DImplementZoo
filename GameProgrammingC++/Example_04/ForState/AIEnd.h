#pragma once
#include "../AIState.h"

class AIComponent;

class AIEnd : public AIState
{
public:
	AIEnd(std::weak_ptr<AIComponent> owner)
		: AIState(owner)
		, m_timer(0.5s)
	{ }

	void Update() override;
	void OnEnter() override;
	void OnExit() override;

	const String GetName() const override
	{
		return U"end";
	}

protected:
	Timer m_timer;
};
