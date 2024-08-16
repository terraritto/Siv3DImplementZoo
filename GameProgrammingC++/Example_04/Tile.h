#pragma once
#include "Actor.h"
#include <vector>

class SpriteComponent;

class Tile : public Actor
{
public:
	friend class GameGrid;
	enum class TileState
	{
		Default,
		Path,
		Start,
		Base
	};

	// constructor
	Tile(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	// Getter/Setter
	void SetTileState(TileState state);

	[[nodiscard]]
	TileState GetTileState() const { return m_tileState; }

	[[nodiscard]]
	const std::weak_ptr<Tile> GetParent() const { return m_parent; }

	void ToggleSelect();

private:
	void UpdateTexture();

private:
	// Path探す用
	std::vector<std::weak_ptr<Tile>> m_adjacent;
	std::weak_ptr<Tile> m_parent;
	double m_f, m_g, m_h;
	bool m_inOpenSet;
	bool m_inClosedSet;
	bool m_blocked;

	std::weak_ptr<SpriteComponent> m_sprite;
	TileState m_tileState;
	bool m_isSelected;
};
