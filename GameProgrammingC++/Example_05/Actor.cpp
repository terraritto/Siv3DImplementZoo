#include "Actor.h"
#include "Game.h"
#include "Component.h"

Actor::Actor(std::weak_ptr<Game> game)
	: m_state(State::Active)
	, m_position(0.0f, 0.0f)
	, m_scale(1.0f)
	, m_rotation(0.0f)
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
		m_worldTransform *= Mat4x4::RotateZ(m_rotation);
		m_worldTransform *= Mat4x4::Translate(Vec3(m_position.x, m_position.y, 0.0));


		for (auto component : m_componentList)
		{
			component->OnUpdateWorldTransform();
		}
	}
}

const Vector2D<float> Actor::GetForward()
{
	Vector2D<float> forward;
	forward.x = Math::Cos(m_rotation);
	forward.y = -Math::Sin(m_rotation);

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
