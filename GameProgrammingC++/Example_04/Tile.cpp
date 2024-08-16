#include "Tile.h"
#include "Game.h"
#include "SpriteComponent.h"

Tile::Tile(std::weak_ptr<Game> game)
	: Actor(game)
	, m_parent()
	, m_f(0.0)
	, m_g(0.0)
	, m_h(0.0)
	, m_blocked(false)
	, m_sprite()
	, m_tileState(TileState::Default)
	, m_isSelected(false)
{
}

void Tile::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// スプライト用のComponentの作成
	m_sprite = game->SpawnSprite<SpriteComponent>(owner).lock();
	UpdateTexture();
}

void Tile::SetTileState(TileState state)
{
	m_tileState = state;
	UpdateTexture();
}

void Tile::ToggleSelect()
{
	m_isSelected = !m_isSelected;
	UpdateTexture();
}

void Tile::UpdateTexture()
{
	auto game = m_game.lock();

	String text;

	switch (m_tileState)
	{
	case TileState::Start:
		text = U"🟫";
		break;

	case TileState::Base:
		text = U"🟦";
		break;

	case TileState::Path:
		text = m_isSelected ? U"🟨" : U"🟫";
		break;

	case TileState::Default:
	default:
		text = m_isSelected ? U"🔲" : U"🟨";
		break;
	}

	auto tex = game->GetEmojoTexture(text);
	m_sprite.lock()->SetTexture(tex);
}
