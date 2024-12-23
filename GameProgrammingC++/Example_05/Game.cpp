﻿#include "Game.h"
#include <Siv3D.hpp>
#include "Actor.h"
#include "Ship.h"
#include "ShaderData.h"

Game::Game()
	: m_isRunning(true)
	, m_actorList()
	, m_pendingActorList()
	, m_spriteList()
	, m_updatingActors(false)
{
}

bool Game::initialize()
{
	// 背景の色を設定する
	Scene::SetBackground(ColorF{ 0.0, 0.0, 1.0, 1.0 });

	LoadData();

    return true;
}

void Game::runLoop()
{
	while (m_isRunning)
	{
		if (System::Update() == false)
		{
			m_isRunning = false;
			break;
		}

		ClearPrint();

		processInput();
		updateGame();
		generateOutput();
	}
}

void Game::shutdown()
{
	UnloadData();
}

Texture Game::GetTexture(const String path)
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

Texture Game::GetEmojoTexture(const String emoji)
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

void Game::RemoveActor(std::weak_ptr<Actor> actor)
{
	auto actorPtr = actor.lock();
	if (!actorPtr) { return; }

	auto iter = std::find(m_pendingActorList.begin(), m_pendingActorList.end(), actorPtr);
	if (iter != m_pendingActorList.end())
	{
		std::iter_swap(iter, m_pendingActorList.end() - 1);
		m_pendingActorList.pop_back();
	}

	iter = std::find(m_actorList.begin(), m_actorList.end(), actorPtr);
	if (iter != m_actorList.end())
	{
		std::iter_swap(iter, m_actorList.end() - 1);
		m_actorList.pop_back();
	}
}

void Game::RemoveSprite()
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

void Game::AddAsteroid(std::weak_ptr<Asteroid> asteroid)
{
	m_asteroidList.emplace_back(asteroid);
}

void Game::RemoveAsteroid()
{
	auto iter = std::find_if(m_asteroidList.begin(), m_asteroidList.end(),
		[](std::weak_ptr<Asteroid>& lhs)
		{
			return lhs.expired();
		}
	);

	if (iter != m_asteroidList.end())
	{
		m_asteroidList.erase(iter);
	}
}

void Game::processInput()
{
	m_updatingActors = true;
	for (auto& actor : m_actorList)
	{
		actor->ProcessInput();
	}
	m_updatingActors = false;
}

void Game::updateGame()
{
	double deltaTime = Scene::DeltaTime();

	// actorの更新
	m_updatingActors = true;
	for (auto& actor : m_actorList)
	{
		actor->Update(deltaTime);
	}
	m_updatingActors = false;

	// pendingのactorを移す
	for (auto& actor : m_pendingActorList)
	{
		actor->ComputeWorldTransform();
		m_actorList.emplace_back(actor);
	}
	m_pendingActorList.clear();

	// いらないActorを削除
	while (true)
	{
		auto iter = std::find_if(m_actorList.begin(), m_actorList.end(),
			[](std::shared_ptr<Actor>& data)
			{
				return data->GetState() == Actor::State::Dead;
			}
		);

		if (iter != m_actorList.end())
		{
			m_actorList.erase(std::remove(m_actorList.begin(), m_actorList.end() ,*iter), m_actorList.end());
			continue;
		}

		break;
	}
}

void Game::generateOutput()
{
	for (auto sprite : m_spriteList)
	{
		if (auto pSprite = sprite.lock())
		{
			pSprite->Draw(m_spriteShader);
		}
	}
}

void Game::LoadData()
{
	Size sceneSize = Window::GetState().virtualSize;

	std::shared_ptr<Ship> ship = SpawnCastActor<Ship>().lock();
	ship->SetPosition({ sceneSize / 2 });
	ship->Initialize();

	constexpr int NumAsteroids = 20;
	for (int i = 0; i < NumAsteroids; i++)
	{
		std::shared_ptr<Asteroid> asteroid = SpawnCastActor<Asteroid>().lock();
		asteroid->Initialize();
	}

	m_spriteShader = std::make_shared<ShaderData>();
	m_spriteShader->LoadShader(U"example/shader/hlsl/default2d.hlsl", U"example/shader/hlsl/default2d.hlsl");
}

void Game::UnloadData()
{
	// actorリソースをクリア
	m_actorList.clear();

	// Textureリソースを解放
	for (auto i : m_textureMap)
	{
		i.second.release();
	}
	m_textureMap.clear();
}
