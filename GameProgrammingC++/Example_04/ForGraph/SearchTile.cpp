#include "SearchTile.h"
#include "../Game.h"

SearchTile::SearchTile(std::weak_ptr<Game> game)
	: Actor(game)
	, m_sprite()
	, m_tileState(TileState::Default)
{
}

void SearchTile::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// スプライト用のComponentの作成
	m_sprite = game->SpawnSprite<SpriteComponent>(owner).lock();
	UpdateTexture();
}

void SearchTile::SetTileState(TileState state)
{
	m_tileState = state;
	UpdateTexture();
}

void SearchTile::UpdateTexture()
{
	auto game = m_game.lock();

	String text;

	switch (m_tileState)
	{
	case TileState::Start:
		// blue
		text = U"🟦";
		break;

	case TileState::End:
		// gold
		text = U"🟨";
		break;

	case TileState::Searched:
		//
		text = U"🟥";
		break;

	case TileState::FindPath:
		// green
		text = U"🟩";
		break;

	case TileState::Wall:
		// brown
		text = U"🟫";
		break;

	case TileState::Default:
	default:
		// white
		text = U"⬜";
		break;
	}

	auto tex = game->GetEmojoTexture(text);
	m_sprite.lock()->SetTexture(tex);

}
