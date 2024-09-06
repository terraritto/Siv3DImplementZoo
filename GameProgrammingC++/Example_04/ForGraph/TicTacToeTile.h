#pragma once
#include "../Actor.h"
#include "Graph.h"
#include <vector>

class SpriteComponent;

class TicTacToeTile : public Actor
{
public:
	friend class TicTacToeBoard;

	// constructor
	TicTacToeTile(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	// Getter/Setter
	void SetTileState(GTState state);

	[[nodiscard]]
	GTState GetTileState() const { return m_tileState; }

private:
	void UpdateTexture();

private:
	std::weak_ptr<SpriteComponent> m_sprite;
	GTState m_tileState;
};
