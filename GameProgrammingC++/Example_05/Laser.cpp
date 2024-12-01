#include "Laser.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"

Laser::Laser(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void Laser::Initialize()
{
	Size sceneSize = Window::GetState().virtualSize;
	auto owner = shared_from_this();
	auto game = m_game.lock();

	SetScale(0.5);

	// 画像の生成
	auto spriteComponent = game->SpawnSprite<SpriteComponent>(owner).lock();
	spriteComponent->SetTexture(game->GetEmojoTexture(U"🔴"));

	auto mc = CreateCastComponent<MoveComponent>(owner).lock();
	mc->SetForwardSpeed(800.0f);

	// Circleの生成
	m_circle = CreateCastComponent<CircleComponent>(owner);
	auto pCircle = m_circle.lock();
	pCircle->SetRadius(10.0f);

	// Timerの設定
	m_timer.set(1.0s);
	m_timer.start();
}

void Laser::UpdateActor(float deltaTime)
{
	if (m_timer.reachedZero())
	{
		SetState(State::Dead);
	}
	else
	{
		auto pGame = GetGame().lock();
		if (!pGame)
		{
			return;
		}

		for (auto& asteroid : pGame->GetAsteroids())
		{
			auto pAsteroid = asteroid.lock();

			if (!pAsteroid)
			{
				return;
			}

			auto pAsteroidCircle = pAsteroid->GetCircle().lock();
			if (pAsteroidCircle->Intersect(m_circle))
			{
				SetState(State::Dead);
				pAsteroid->SetState(State::Dead);
				break;
			}
		}
	}
}
