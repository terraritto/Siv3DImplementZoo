#pragma once
#include <Siv3D.hpp>
#include "Component.h"
#include "SpriteComponent.h"
#include "Renderer.h"

class Actor;
class SpriteComponent;
class Ship;
class ShaderData;

class Game : public std::enable_shared_from_this<Game>
{
public:
	Game();

	bool initialize();
	void runLoop();
	void shutdown();

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

	template<class T>
	Array<std::weak_ptr<T>> GetActorsFromClass()
	{
		Array<std::weak_ptr<T>> actors;

		for (auto& actor : m_actorList)
		{
			std::shared_ptr<T> target = std::dynamic_pointer_cast<T>(actor);
			if (target)
			{
				actors.push_back(target);
			}
		}

		return actors;
	}

	void RemoveActor(std::weak_ptr<Actor> actor);

	std::shared_ptr<Renderer> GetRenderer() { return m_renderer; }

private:
	void processInput();
	void updateGame();
	void generateOutput();
	void LoadData();
	void UnloadData();

	bool m_isRunning;

	Array<std::shared_ptr<Actor>> m_actorList;
	Array<std::shared_ptr<Actor>> m_pendingActorList;
	bool m_updatingActors; // Actorの更新をしているか？

	std::shared_ptr<Renderer> m_renderer;
};
