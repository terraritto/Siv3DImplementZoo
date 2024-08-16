#include "Food.h"
#include "../Game.h"

Food::Food(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void Food::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// スプライト用のComponentの作成
	m_sprite = game->SpawnSprite<SpriteComponent>(owner).lock();

	// Textureを設定
	auto tex = game->GetEmojoTexture(U"🥬");
	m_sprite.lock()->SetTexture(tex);
}
