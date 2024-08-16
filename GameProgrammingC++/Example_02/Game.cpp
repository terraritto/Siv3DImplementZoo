#include "Game.h"
#include <Siv3D.hpp>
#include "Actor.h"
#include "Ship.h"
#include "AnimSpriteComponent.h"
#include "BGSpriteComponent.h"

Game::Game()
	: m_isRunning(true)
	, m_actorList()
	, m_pendingActorList()
	, m_spriteList()
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

void Game::RemoveSprite(std::weak_ptr<SpriteComponent> sprite)
{
	if (auto pSprite = sprite.lock())
	{
		auto iter = std::find_if(m_spriteList.begin(), m_spriteList.end(), [pSprite](std::weak_ptr<SpriteComponent>& ptr)
			{
				if (auto sp = ptr.lock())
				{
					return sp == pSprite;
				}
				return false;
			});
		m_spriteList.erase(iter);
	}
}

void Game::processInput()
{
	if (auto pShip = m_ship.lock())
	{
		pShip->ProcessKeyboard();
	}
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
		m_actorList.emplace_back(actor);
	}
	m_pendingActorList.clear();

	// いらないActorを削除
	auto iter = m_actorList.begin();
	while (iter != m_actorList.end())
	{
		if ((*iter)->GetState() == Actor::State::Dead)
		{
			m_actorList.erase(iter);
			continue;
		}
		iter++;
	}
}

void Game::generateOutput()
{
	for (auto sprite : m_spriteList)
	{
		if (auto pSprite = sprite.lock())
		{
			pSprite->Draw();
		}
	}
}

void Game::LoadData()
{
	Size sceneSize = Window::GetState().virtualSize;

	/*
	auto spriteActor = SpawnActor<Actor>().lock();
	spriteActor->SetPosition({ sceneSize / 2 });

	auto spriteComponent = SpawnSprite<SpriteComponent>(spriteActor, 10).lock();
	spriteComponent->SetTexture(GetTexture(U"example/siv3d-kun.png"));
	*/

	/*
	auto spriteActor = SpawnActor<Actor>().lock();
	spriteActor->SetPosition({ sceneSize / 2 });

	auto spriteComponent = SpawnCastSprite<AnimSpriteComponent>(spriteActor, 10).lock();
	Texture spriteSheetTexture = GetTexture(U"example/spritesheet/siv3d-kun-16.png");
	Array<TextureRegion> spriteSheetList;
	const size_t row = 6, col = 4;
	for (size_t r = 0; r < row; ++r)
	{
		for (size_t c = 0; c < col; ++c)
		{
			spriteSheetList.push_back(spriteSheetTexture.uv(
				RectF{ r / static_cast<double>(row), c / static_cast<double>(col),
				1 / static_cast<double>(row), 1 / static_cast<double>(col)}));
		}
	}

	spriteComponent->SetAnimationFps(1.0f);
	spriteComponent->SetAnimTextures(spriteSheetList);
	*/
	
	auto spriteActor = SpawnActor<Actor>().lock();
	spriteActor->SetPosition({ 0, 0 });

	auto spriteComponent = SpawnCastSprite<BGSpriteComponent>(spriteActor, 10).lock();
	spriteComponent->SetScrollSpeed(-100.0f);
	spriteComponent->SetScreenSize(sceneSize);
	spriteComponent->SetBGTextures({
		GetTexture(U"example/windmill.png"),
		GetTexture(U"example/bay.jpg")
		});

	/*
	std::shared_ptr<Ship> ship = SpawnCastActor<Ship>().lock();
	ship->SetPosition({ sceneSize / 2 });

	auto spriteComponent = SpawnSprite<SpriteComponent>(ship).lock();
	spriteComponent->SetTexture(GetEmojoTexture(U"🚀"));
	m_ship = ship;
	*/
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
