#pragma once
#include "Component.h"

class AIState;

class AIComponent : public Component
{
public:
	AIComponent(std::weak_ptr<Actor> owner);

	void Update(float deltaTime) override;
	void ChangeState(const String& name);

	void RegisterState(std::shared_ptr<AIState> state);

protected:
	std::unordered_map<String, std::shared_ptr<AIState>> m_stateMap;
	std::shared_ptr<AIState> m_currentState;
};
