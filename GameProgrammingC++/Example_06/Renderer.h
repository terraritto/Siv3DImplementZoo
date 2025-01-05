#pragma once
#include <Siv3D.hpp>
#include "Component.h"
#include "SpriteComponent.h"
#include "DefaultModel.h"
#include "MeshComponent.h"

class Renderer : public std::enable_shared_from_this<Renderer>
{
public:
	Renderer(std::weak_ptr<Game> game);
	~Renderer();

	bool Initialize();
	void Shutdown();
	void UnloadData();

	void Draw();

	// Sprite
	void AddSprite(std::shared_ptr<SpriteComponent> sprite);
	void RemoveSprite();

	template<class T, class... Types>
	std::weak_ptr<SpriteComponent> SpawnSprite(std::weak_ptr<Actor> actor, Types... values)
	{
		std::shared_ptr<SpriteComponent> sprite = std::dynamic_pointer_cast<SpriteComponent>(CreateComponent<T, Types...>(actor, values...).lock());
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

		return sprite;
	}

	template<class T, class... Types>
	std::weak_ptr<T> SpawnCastSprite(std::weak_ptr<Actor> actor, Types... values)
	{
		std::shared_ptr<SpriteComponent> sprite = std::dynamic_pointer_cast<SpriteComponent>(CreateComponent<T, Types...>(actor, values...).lock());
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

		return std::dynamic_pointer_cast<T>(sprite);
	}

	// Mesh
	void AddMeshComponent(std::shared_ptr<MeshComponent> meshComp);
	void RemoveMeshComponent();

	// Getter
	Texture GetTexture(const String& path);
	Texture GetEmojiTexture(const String& emoji);
	std::weak_ptr<DefaultModel> GetModel(const String& path);

private:
	bool LoadShaders();

private:
	std::weak_ptr<Game> m_game;
	float m_screenWidth, m_screenHeight;
	ColorF m_backgroundColor;

	// todo: cameraの章で正しいものに直す
	// camera
	DebugCamera3D m_camera;

	// mesh
	std::map<String, std::shared_ptr<DefaultModel>> m_modelMap;
	Array<std::weak_ptr<MeshComponent>> m_meshComps;

	// sprite
	Array<std::weak_ptr<SpriteComponent>> m_spriteList;
	std::shared_ptr<ShaderData> m_spriteShader;

	// texture
	std::map<String, Texture> m_textureMap;

	std::unique_ptr<MSRenderTexture> m_renderTexture;
};
