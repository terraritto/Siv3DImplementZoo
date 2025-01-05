#pragma once
#include <Siv3D.hpp>
#include "ShaderData.h"

class Renderer;

class DefaultModel
{
public:
	DefaultModel();
	~DefaultModel();

	// ロード/アンロード
	bool Load(const String& fileName, std::shared_ptr<Renderer> renderer);
	void Unload();

	// 描画
	void Draw(Mat4x4 mtx);

	// 取得
	String GetTextureName(size_t index);
	const std::weak_ptr<ShaderData> GetShader() const { return m_shader; }
	void SetShaderData(std::shared_ptr<ShaderData> shader) { m_shader = shader; }
	Sphere GetBoundingSphere() const { return m_model->boundingSphere(); }

private:
	std::unique_ptr<Model> m_model;
	Array<String> m_textureNames;
	std::shared_ptr<ShaderData> m_shader;
};
