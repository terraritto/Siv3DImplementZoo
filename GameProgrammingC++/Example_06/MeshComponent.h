#pragma once
#include "Component.h"

class DefaultModel;
class ShaderData;

class MeshComponent : public Component
{
public:
	MeshComponent(std::weak_ptr<Actor> owner);
	~MeshComponent();

	virtual void Draw(std::shared_ptr<ShaderData> shader = nullptr);
	virtual void SetModel(class std::weak_ptr<DefaultModel> model) { m_model = model; }
	void SetTextureIndex(size_t index) { m_textureIndex = index; }

protected:
	std::weak_ptr<DefaultModel> m_model;
	size_t m_textureIndex;
};
