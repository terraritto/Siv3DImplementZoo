#pragma once
#include "../Actor.h"
#include <vector>

class SpriteComponent;

class TicTacToeTile : public Actor
{
public:
	friend class TicTacToeBoard;
	enum class TileState
	{
		None,
		Square,
		Cross
	};

	// constructor
	TicTacToeTile(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	// Getter/Setter
	void SetTileState(TileState state);

	[[nodiscard]]
	TileState GetTileState() const { return m_tileState; }

private:
	void UpdateTexture();

private:
	std::weak_ptr<SpriteComponent> m_sprite;
	TileState m_tileState;
};
