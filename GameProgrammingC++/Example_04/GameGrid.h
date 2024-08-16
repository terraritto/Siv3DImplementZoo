#pragma once
#include "Actor.h"
#include <vector>

class Tile;

class GameGrid : public Actor
{
public:
	// constructor
	GameGrid(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	// マウスのクリック処理
	void ProcessClick(int x, int y);

	std::weak_ptr<Tile> GetStartTile() { return m_tiles[3][0]; }
	std::weak_ptr<Tile> GetEndTile() { return m_tiles[3][15]; }

private:
	// 特定のタイルを選択
	void SelectTile(int row, int col);

private:
	// すべてのタイル
	Array<Array<std::shared_ptr<Tile>>> m_tiles;

	// セレクトされているタイル
	std::shared_ptr<Tile> m_selectedTile;

private:
	// Tileの数
	const int NumRows = 7;
	const int NumCols = 16;

	// 開始Y軸の位置
	const double StartX = 100.0;
	const double StartY = 192.0;

	// Tileのサイズ
	const double TileSize = 32.0;
};
