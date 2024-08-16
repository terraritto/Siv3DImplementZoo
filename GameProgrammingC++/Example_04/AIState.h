#pragma once
#include <memory>
#include <Siv3D.hpp>

class AIComponent;

class AIState
{
public:
	AIState(std::weak_ptr<AIComponent> owner)
		: m_owner(owner)
	{}

	virtual void Update() = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual const String GetName() const = 0;

protected:
	std::weak_ptr<AIComponent> m_owner;
};

class AIPatrol : public AIState
{
public:
	using AIState::AIState;

	virtual void Update() = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual const String GetName() const = 0;

protected:
	std::weak_ptr<AIComponent> m_owner;
};
