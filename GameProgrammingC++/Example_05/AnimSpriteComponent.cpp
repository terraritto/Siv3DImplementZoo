#include "AnimSpriteComponent.h"
#include "ShaderData.h"

AnimSpriteComponent::AnimSpriteComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
	, m_currentFrame(0.0f)
	, m_animationFps(24.0f)
{
	
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (m_animTextureList.size() > 0)
	{
		m_currentFrame += m_animationFps * deltaTime;

		while (m_currentFrame >= m_animTextureList.size())
		{
			m_currentFrame -= m_animTextureList.size();
		}
	}
}

void AnimSpriteComponent::Draw(std::shared_ptr<ShaderData> shader)
{
	if (m_animTextureList.size() > 0)
	{
		auto owner = m_owner.lock();
		TextureRegion texture = m_animTextureList[static_cast<int>(m_currentFrame)];
		if (shader)
		{
			const ScopedCustomShader2D scopedShader{ *(shader->GetVertex().lock()), *(shader->GetPixel().lock()) };
			texture.scaled(owner->GetScale()).drawAt(owner->GetPosition());
		}
		else
		{
			texture.scaled(owner->GetScale()).drawAt(owner->GetPosition());
		}
	}
}

void AnimSpriteComponent::SetAnimTextures(const Array<TextureRegion>& textureList)
{
	m_animTextureList = textureList;

	if (m_animTextureList.size() > 0)
	{
		m_currentFrame = 0.0f;
	}
}
