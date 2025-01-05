#include "Actor.h"
#include "Game.h"
#include "Component.h"

Actor::Actor(std::weak_ptr<Game> game)
	: m_state(State::Active)
	, m_position(Vec3::Zero())
	, m_scale(1.0f)
	, m_rotation(Quaternion::Identity())
	, m_game(game)
	, m_recomputeWorldTransform(true)
{
}

Actor::~Actor()
{
	m_componentList.clear();
}

void Actor::Update(float deltaTime)
{
	if (m_state == State::Active)
	{
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);

		ComputeWorldTransform();
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto& comp : m_componentList)
	{
		comp->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ProcessInput()
{
	if (m_state == State::Active)
	{
		// Component側の入力制御
		for (auto comp : m_componentList)
		{
			comp->ProcessInput();
		}

		// Actor側の入力制御
		ActorInput();
	}
}

void Actor::ActorInput()
{
}

void Actor::ComputeWorldTransform()
{
	if (m_recomputeWorldTransform)
	{
		m_recomputeWorldTransform = false;

		m_worldTransform = Mat4x4::Scale(m_scale);
		m_worldTransform *= Mat4x4::Rotate(m_rotation);
		m_worldTransform *= Mat4x4::Translate(m_position);


		for (auto component : m_componentList)
		{
			component->OnUpdateWorldTransform();
		}
	}
}

const Vec3 Actor::GetForward()
{
	Vec3 forward = Vec3::UnitX() * m_rotation;
	return forward;
}

void Actor::AddComponent(std::shared_ptr<Component> component)
{
	int myOrder = component->getUpdateOrder();
	auto iter = m_componentList.begin();
	for (; iter != m_componentList.end(); iter++)
	{
		if (myOrder < (*iter)->getUpdateOrder())
		{
			break;
		}
	}

	m_componentList.insert(iter, component);
}
