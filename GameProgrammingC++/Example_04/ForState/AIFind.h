#pragma once
#include "../AIState.h"

class AIComponent;

class AIFind : public AIState
{
public:
	AIFind(std::weak_ptr<AIComponent> owner)
		: AIState(owner)
		, m_timer{0.5s}
	{ }

	void Update() override;
	void OnEnter() override;
	void OnExit() override;

	const String GetName() const override
	{
		return U"find";
	}

protected:
	Timer m_timer;
};
