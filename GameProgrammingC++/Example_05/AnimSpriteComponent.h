#pragma once
#include "SpriteComponent.h"

class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(std::weak_ptr<Actor> owner, int drawOrder = 100);

	virtual void Update(float deltaTime) override;
	virtual void Draw(std::shared_ptr<ShaderData> shader = nullptr) override;

	void SetAnimTextures(const Array<TextureRegion>& textureList);

	// animationのFPSを設定
	void SetAnimationFps(float fps) { m_animationFps = fps; }

	[[nodiscard]]
	float GetAnimationFps() const { return m_animationFps; }

protected:
	Array<TextureRegion> m_animTextureList;
	float m_currentFrame;
	float m_animationFps;
};
