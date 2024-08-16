﻿#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: Component(owner)
	, m_drawOrder(drawOrder)
	, m_texWidth(0)
	, m_texHeight(0)
{
}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::Draw()
{
	auto owner = m_owner.lock();

	if (owner)
	{
		// 位置を中心にして描画
		m_texture.scaled(owner->GetScale()).drawAt(owner->GetPosition());
	}
}

void SpriteComponent::SetTexture(const Texture tex)
{
	m_texture = tex;
	m_texWidth = m_texture.width();
	m_texHeight = m_texture.height();
}
