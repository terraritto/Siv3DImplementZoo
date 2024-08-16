#include "GameGrid.h"
#include "Tile.h"

GameGrid::GameGrid(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void GameGrid::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// Tileの初期化
	m_tiles.resize(NumRows);
	for (int i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].resize(NumCols);
	}

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			m_tiles[i][j] = std::make_shared<Tile>(GetGame());
			m_tiles[i][j]->Initialize();
			m_tiles[i][j]->SetScale(0.27);
			m_tiles[i][j]->SetPosition(Float2{ StartX + TileSize / 2.0 + j * TileSize, StartY + i * TileSize });
		}
	}

	GetStartTile().lock()->SetTileState(Tile::TileState::Start);
	GetEndTile().lock()->SetTileState(Tile::TileState::Base);
}

void GameGrid::ProcessClick(int x, int y)
{
	// (0,0)の位置にずらしておく
	x -= static_cast<int>(StartX - TileSize / 2.0);
	y -= static_cast<int>(StartY - TileSize / 2.0);

	if (y >= 0)
	{
		// Tileのindexに変換
		x /= static_cast<int>(TileSize);
		y /= static_cast<int>(TileSize);

		// 範囲内にあるか？
		bool isSelectedX = x >= 0 && x < static_cast<int>(NumCols);
		bool isSelectedY = y >= 0 && y < static_cast<int>(NumRows);

		if (isSelectedX && isSelectedY)
		{
			SelectTile(y, x);
		}
	}
}

void GameGrid::SelectTile(int row, int col)
{
	auto tileState = m_tiles[row][col]->GetTileState();

	// セレクトしていいTileかどうかの判定
	if (tileState != Tile::TileState::Start && tileState != Tile::TileState::Base)
	{
		// セレクトがある場合はセレクトを外す
		if (m_selectedTile)
		{
			m_selectedTile->ToggleSelect();
		}

		// セレクトを付ける
		m_selectedTile = m_tiles[row][col];
		m_selectedTile->ToggleSelect();
	}
}
