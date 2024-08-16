#include "AIComponent.h"
#include "AIState.h"

AIComponent::AIComponent(std::weak_ptr<Actor> owner)
	: Component(owner)
	, m_currentState(nullptr)
{
}

void AIComponent::Update(float deltaTime)
{
	if (m_currentState)
	{
		m_currentState->Update();
	}
}

void AIComponent::ChangeState(const String& name)
{
	// 終了処理をしておく
	if (m_currentState)
	{
		m_currentState->OnExit();
	}

	auto iter = m_stateMap.find(name);
	if (iter != m_stateMap.end())
	{
		// 次のstateに切り替え後、開始処理
		m_currentState = iter->second;
		m_currentState->OnEnter();
	}
	else
	{
		Logger << U"Could not find AIState " << name << U" in state map.";
		m_currentState.reset();
	}
}

void AIComponent::RegisterState(std::shared_ptr<AIState> state)
{
	m_stateMap.emplace(state->GetName(), state);
}
