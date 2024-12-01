#pragma once
#include "SpriteComponent.h"

class BGSpriteComponent : public SpriteComponent
{
public:
	BGSpriteComponent(std::weak_ptr<Actor> owner, int drawOrder = 10);

	virtual void Update(float deltaTime) override;
	virtual void Draw(std::shared_ptr<ShaderData> shader = nullptr) override;

	void SetBGTextures(const Array<Texture>& textureList);
	void SetScreenSize(const Vec2& size) { m_screenSize = size; }
	void SetScrollSpeed(float speed) { m_scrollSpeed = speed; }

	[[nodiscard]]
	float GetScrollSpeed() const { return m_scrollSpeed; }

protected:
	struct BGTexture
	{
		Texture m_texture;
		Vec2 m_offset;
	};

	Array<BGTexture> m_textureList;
	Vec2 m_screenSize;
	float m_scrollSpeed;
};
