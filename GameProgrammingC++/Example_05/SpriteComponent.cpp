#include "SpriteComponent.h"
#include "Game.h"
#include "ShaderData.h"

SpriteComponent::SpriteComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: Component(owner)
	, m_drawOrder(drawOrder)
	, m_texWidth(0)
	, m_texHeight(0)
{
}

SpriteComponent::~SpriteComponent()
{
	if (auto game = m_game.lock())
	{
		game->RemoveSprite();
	}
}

void SpriteComponent::Draw(std::shared_ptr<ShaderData> shader)
{
	auto owner = m_owner.lock();

	if (owner)
	{
		//const ScopedRenderStates2D blend{ BlendState::Additive };

		if (shader)
		{
			const ScopedCustomShader2D scopedShader
			{
				*(shader->GetVertex().lock()),
				*(shader->GetPixel().lock())
			};

			m_texture.scaled(owner->GetScale()).rotated(owner->GetRotation()).drawAt(owner->GetPosition());
		}
		else
		{
			m_texture.scaled(owner->GetScale()).rotated(owner->GetRotation()).drawAt(owner->GetPosition());
		}
	}
}

void SpriteComponent::SetTexture(const Texture tex)
{
	m_texture = tex;
	m_texWidth = m_texture.width();
	m_texHeight = m_texture.height();
}
