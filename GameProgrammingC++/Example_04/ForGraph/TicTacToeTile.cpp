#include "TicTacToeTile.h"
#include "../Game.h"

TicTacToeTile::TicTacToeTile(std::weak_ptr<Game> game)
	: Actor(game)
	, m_sprite()
	, m_tileState(TileState::None)
{
}

void TicTacToeTile::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// スプライト用のComponentの作成
	m_sprite = game->SpawnSprite<SpriteComponent>(owner).lock();
	UpdateTexture();
}

void TicTacToeTile::SetTileState(TileState state)
{
	m_tileState = state;
	UpdateTexture();
}

void TicTacToeTile::UpdateTexture()
{
	auto game = m_game.lock();

	String text;

	switch (m_tileState)
	{
	case TileState::Square:
		// blue
		text = U"🟦";
		break;

	case TileState::Cross:
		// gold
		text = U"🟨";
		break;

	case TileState::None:
	default:
		// white
		text = U"⬜";
		break;
	}

	auto tex = game->GetEmojoTexture(text);
	m_sprite.lock()->SetTexture(tex);
}
