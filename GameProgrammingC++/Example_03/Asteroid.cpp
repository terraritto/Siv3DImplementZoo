#include "Asteroid.h"
#include "Game.h"
#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"

Asteroid::Asteroid(std::weak_ptr<Game> game)
	: Actor(game)
{
}

Asteroid::~Asteroid()
{
	auto game = m_game.lock();

	game->RemoveAsteroid();
}

void Asteroid::Initialize()
{
	Size sceneSize = Window::GetState().virtualSize;
	auto owner = shared_from_this();
	auto game = m_game.lock();

	Vector2D<float> randomPosition = RandomVec2() * sceneSize.xy();
	SetPosition(randomPosition);
	SetRotation(Random() * Math::Pi * 2.0f);
	SetScale(0.5);

	// 画像の生成
	auto spriteComponent = game->SpawnSprite<SpriteComponent>(owner).lock();
	spriteComponent->SetTexture(game->GetEmojoTexture(U"⭐"));

	auto mc = CreateCastComponent<MoveComponent>(owner).lock();
	mc->SetForwardSpeed(150.0f);

	// Circleの生成
	m_circle = CreateCastComponent<CircleComponent>(owner);
	auto pCircle = m_circle.lock();
	pCircle->SetRadius(40.0f);

	game->AddAsteroid(std::dynamic_pointer_cast<Asteroid>(owner));
}
