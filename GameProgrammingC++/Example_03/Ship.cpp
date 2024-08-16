#include "Ship.h"
#include "Game.h"
#include "Laser.h"
#include "SpriteComponent.h"
#include "InputComponent.h"

Ship::Ship(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void Ship::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
}

void Ship::Initialize()
{
	Size sceneSize = Window::GetState().virtualSize;
	auto owner = shared_from_this();
	auto game = m_game.lock();

	SetScale(0.5);

	// 画像の生成
	auto spriteComponent = game->SpawnSprite<SpriteComponent>(owner, 150).lock();
	spriteComponent->SetTexture(game->GetEmojoTexture(U"🚀"));

	// 入力の生成
	auto inputComponent = CreateCastComponent<InputComponent>(owner).lock();
	inputComponent->SetMaxForwardSpeed(300.0f);
	inputComponent->SetMaxAngularSpeed(Math::TwoPiF);

	// Timerの設定
	m_laserTimer.set(0.5s);
	m_laserTimer.setRemaining(-0.1s);
	m_laserTimer.start();
}

void Ship::ActorInput()
{
	if (KeySpace.pressed() && m_laserTimer.reachedZero())
	{
		auto game = m_game.lock();
		auto owner = shared_from_this();
		auto pLaser = game->SpawnActor<Laser>().lock();
		pLaser->Initialize();

		pLaser->SetPosition(GetPosition());
		pLaser->SetRotation(GetRotation());

		m_laserTimer.restart();
	}
}
