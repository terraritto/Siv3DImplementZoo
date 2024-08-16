#include "BGSpriteComponent.h"

BGSpriteComponent::BGSpriteComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
	, m_scrollSpeed(0.0f)
{
}

void BGSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);
	for (auto& bg : m_textureList)
	{
		bg.m_offset.x += m_scrollSpeed * deltaTime;

		if (bg.m_offset.x <= -m_screenSize.x)
		{
			bg.m_offset.x = (m_textureList.size() - 1) * m_screenSize.x - 1;
		}
	}
}

void BGSpriteComponent::Draw()
{
	for (auto& bg : m_textureList)
	{
		auto owner = m_owner.lock();
		bg.m_texture.resized(m_screenSize).draw(owner->GetPosition() + bg.m_offset);
	}
}

void BGSpriteComponent::SetBGTextures(const Array<Texture>& textureList)
{
	int count = 0;
	for (auto& tex : textureList)
	{
		BGTexture temp;
		temp.m_texture = tex;
		temp.m_offset.x = count * m_screenSize.x;
		temp.m_offset.y = 0;
		m_textureList.emplace_back(temp);
		count++;
	}
}
