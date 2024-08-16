#pragma once
#include "../Actor.h"
#include <vector>

class SpriteComponent;

class SearchTile : public Actor
{
public:
	friend class SearchGrid;
	enum class TileState
	{
		Default,
		Wall,
		FindPath,
		Start,
		End,
		Searched
	};

	// constructor
	SearchTile(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	// Getter/Setter
	void SetTileState(TileState state);

	[[nodiscard]]
	TileState GetTileState() const { return m_tileState; }

private:
	void UpdateTexture();

private:
	// Path探す用
	std::vector<std::weak_ptr<SearchTile>> m_adjacent;

	std::weak_ptr<SpriteComponent> m_sprite;
	TileState m_tileState;
};
