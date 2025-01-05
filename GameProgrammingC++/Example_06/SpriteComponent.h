#pragma once
#include "Component.h"

class ShaderData;

class SpriteComponent : public Component
{
public:
	SpriteComponent(std::weak_ptr<Actor> owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(std::shared_ptr<ShaderData> shader = nullptr);
	virtual void SetTexture(const Texture texPath);

	[[nodiscard]]
	int GetDrawOrder() const { return m_drawOrder; }

	[[nodiscard]]
	int GetTexWidth() const { return m_texWidth; }

	[[nodiscard]]
	int GetTexHeight() const { return m_texHeight; }

protected:
	Texture m_texture;

	int m_drawOrder;
	int m_texWidth;
	int m_texHeight;
};
