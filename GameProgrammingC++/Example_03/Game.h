#pragma once
#include <Siv3D.hpp>
#include "Asteroid.h"
#include "Component.h"
#include "SpriteComponent.h"

class Actor;
class SpriteComponent;
class Ship;

class Game : public std::enable_shared_from_this<Game>
{
public:
	Game();

	bool initialize();
	void runLoop();
	void shutdown();

	Texture GetTexture(const String path);
	Texture GetEmojoTexture(const String emoji);

	// Actor
	template<class T>
	std::weak_ptr<Actor> SpawnActor()
	{
		auto ptr = shared_from_this();
		std::shared_ptr<Actor> actor = std::make_shared<T>(ptr);

		if (m_updatingActors)
		{
			m_pendingActorList.emplace_back(actor);
		}
		else
		{
			m_actorList.emplace_back(actor);
		}

		return actor;
	}

	template<class T>
	std::weak_ptr<T> SpawnCastActor()
	{
		auto owner = shared_from_this();

		std::shared_ptr<T> actor = std::make_shared<T>(owner);

		if (m_updatingActors)
		{
			m_pendingActorList.emplace_back(actor);
		}
		else
		{
			m_actorList.emplace_back(actor);
		}

		return actor;
	}

	void RemoveActor(std::weak_ptr<Actor> actor);


	// Sprite
	void AddSprite(std::shared_ptr<SpriteComponent> sprite)
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

	void RemoveSprite();

	// Asteroid用
	void AddAsteroid(std::weak_ptr<Asteroid> asteroid);
	void RemoveAsteroid();
	std::vector<std::weak_ptr<Asteroid>> GetAsteroids() { return m_asteroidList; }

private:
	void processInput();
	void updateGame();
	void generateOutput();
	void LoadData();
	void UnloadData();

	bool m_isRunning;

	std::map<String, Texture> m_textureMap;

	Array<std::shared_ptr<Actor>> m_actorList;
	Array<std::shared_ptr<Actor>> m_pendingActorList;
	Array<std::weak_ptr<SpriteComponent>> m_spriteList;
	bool m_updatingActors; // Actorの更新をしているか？

	std::weak_ptr<Ship> m_ship;
	std::vector<std::weak_ptr<Asteroid>> m_asteroidList;
};
