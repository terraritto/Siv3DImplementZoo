#include "DefaultModel.h"

DefaultModel::DefaultModel()
{
}

DefaultModel::~DefaultModel()
{
}

bool DefaultModel::Load(const String& fileName, std::shared_ptr<Renderer> renderer)
{
	m_model = std::make_unique<Model>(fileName);

	// 読み込み失敗
	if (m_model->isEmpty()) { return false; }

	// Texture
	m_textureNames = m_model->diffuseTextureNames();

    return true;
}

void DefaultModel::Unload()
{
	m_model->release();
	m_model.reset();
}

void DefaultModel::Draw(Mat4x4 mtx)
{
	m_model->draw(mtx);
}

String DefaultModel::GetTextureName(size_t index)
{
	if (index < m_textureNames.size())
	{
		return m_textureNames[index];
	}

	return String{};
}
