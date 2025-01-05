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
		game->GetRenderer()->RemoveSprite();
	}
}

void SpriteComponent::Draw(std::shared_ptr<ShaderData> shader)
{
	auto owner = m_owner.lock();

	if (owner)
	{
		// 軸と角を得た後、Crossで方向を補正
		auto [a, b] = owner->GetRotation().toAxisAngle();
		Vec3 axis = a.cross(Vec3::UnitX());
		float angle = b * (axis.y < 0 ? -1 : 1);

		if (shader)
		{
			const ScopedCustomShader2D scopedShader
			{
				*(shader->GetVertex().lock()),
				*(shader->GetPixel().lock())
			};

			m_texture.scaled(owner->GetScale()).rotated(angle).drawAt(owner->GetPosition().xy());
		}
		else
		{
			m_texture.scaled(owner->GetScale()).rotated(angle).drawAt(owner->GetPosition().xy());
		}
	}
}

void SpriteComponent::SetTexture(const Texture tex)
{
	m_texture = tex;
	m_texWidth = m_texture.width();
	m_texHeight = m_texture.height();
}
