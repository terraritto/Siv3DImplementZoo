#include "MeshComponent.h"
#include "Game.h"
#include "Actor.h"
#include "ShaderData.h"

MeshComponent::MeshComponent(std::weak_ptr<Actor> owner)
	: Component(owner)
	, m_model()
	, m_textureIndex(0)
{
}

MeshComponent::~MeshComponent()
{
	auto p = m_game.lock();
	if (p)
	{
		p->GetRenderer()->RemoveMeshComponent();
	}
}

void MeshComponent::Draw(std::shared_ptr<ShaderData> shader)
{
	auto pModel = m_model.lock();
	if (pModel)
	{
		pModel->Draw(m_owner.lock()->GetWorldTransform());
	}
}
