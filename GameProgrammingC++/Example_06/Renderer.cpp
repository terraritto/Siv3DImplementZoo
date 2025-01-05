#include "Renderer.h"

Renderer::Renderer(std::weak_ptr<Game> game)
	: m_game(game)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize()
{
	m_camera = DebugCamera3D{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };

	// 背景の色を設定する
	m_backgroundColor = ColorF{ 0.0, 0.0, 1.0, 1.0 };
	Scene::SetBackground(m_backgroundColor);

	auto screenSize = Window::GetState().virtualSize;
	m_screenWidth = screenSize.x;
	m_screenHeight = screenSize.y;

	Window::SetTitle(U"Example:6");

	// RenderTextureの用意
	m_renderTexture = std::make_unique<MSRenderTexture>(Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes);

	if (!LoadShaders())
	{
		return false;
	}

	return true;
}

void Renderer::Shutdown()
{
	m_spriteShader.reset();
}

void Renderer::UnloadData()
{
	// Textureリソースを解放
	for (auto i : m_textureMap)
	{
		i.second.release();
	}
	m_textureMap.clear();
}

void Renderer::Draw()
{
	// Cameraを設定
	m_camera.update(4.0);
	Graphics3D::SetCameraTransform(m_camera);
	
	// Mesh
	{
		const ScopedRenderTarget3D target{ m_renderTexture->clear(m_backgroundColor) };

		// 3Dの描画
		for (auto mc : m_meshComps)
		{
			auto p = mc.lock();
			if (p) { p->Draw(); }
		}
	}

	// RenderTextureを2Dへ
	{
		Graphics3D::Flush();
		m_renderTexture->resolve();
		Shader::LinearToScreen(*m_renderTexture.get());
	}

	// Spriteの描画
	for (auto sprite : m_spriteList)
	{
		auto pSprite = sprite.lock();
		if (pSprite)
		{
			pSprite->Draw(this->m_spriteShader);
		}
	}
}

void Renderer::AddSprite(std::shared_ptr<SpriteComponent> sprite)
{
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = m_spriteList.begin();
	for (; iter != m_spriteList.end(); ++iter)
	{
		auto data = (*iter).lock();
		if (myDrawOrder < data->GetDrawOrder())
		{
			break;
		}
	}

	m_spriteList.insert(iter, sprite);
}

void Renderer::RemoveSprite()
{
	auto iter = std::remove_if(m_spriteList.begin(), m_spriteList.end(), [](std::weak_ptr<SpriteComponent>& ptr)
	{
	return ptr.expired();
	});

	if (iter != m_spriteList.end())
	{
		m_spriteList.erase(iter, m_spriteList.end());
	}
}

void Renderer::AddMeshComponent(std::shared_ptr<MeshComponent> meshComp)
{
	m_meshComps.emplace_back(meshComp);
}

void Renderer::RemoveMeshComponent()
{
	auto iter = std::remove_if(m_meshComps.begin(), m_meshComps.end(), [](std::weak_ptr<MeshComponent>& ptr)
	{
		return ptr.expired();
	});

	if (iter != m_meshComps.end())
	{
		m_meshComps.erase(iter, m_meshComps.end());
	}
}

Texture Renderer::GetTexture(const String& path)
{
	Texture tex;

	auto iter = m_textureMap.find(path);
	if (iter != m_textureMap.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = Texture{ path };

		// ない場合は空を返す
		if (tex.isEmpty())
		{
			return tex;
		}

		m_textureMap.emplace(path, tex);
	}

	return tex;
}

Texture Renderer::GetEmojiTexture(const String& emoji)
{
	Texture tex;

	auto iter = m_textureMap.find(emoji);
	if (iter != m_textureMap.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = Texture{ Emoji(emoji) };

		// ない場合は空を返す
		if (tex.isEmpty())
		{
			return tex;
		}

		m_textureMap.emplace(emoji, tex);
	}

	return tex;
}

std::weak_ptr<DefaultModel> Renderer::GetModel(const String& fileName)
{
	std::shared_ptr<DefaultModel> model = nullptr;
	auto iter = m_modelMap.find(fileName);
	if (iter != m_modelMap.end())
	{
		model = iter->second;
	}
	else
	{
		// まだ登録されていなければ登録を行う
		model = std::make_shared<DefaultModel>();
		if (model->Load(fileName, this->shared_from_this()))
		{
			m_modelMap.emplace(fileName, model);
		}
		else

		{
			model.reset();
		}
	}

	return model;
}

bool Renderer::LoadShaders()
{
	// Sprite用のShader
	m_spriteShader = std::make_shared<ShaderData>();
	m_spriteShader->LoadShader(U"example/shader/hlsl/default2d.hlsl", U"example/shader/hlsl/default2d.hlsl");

	return true;
}
