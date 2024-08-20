#pragma once
#include "../Actor.h"
#include "TicTacToeTile.h"
#include <vector>

class TicTacToeBoard : public Actor
{
public:
	// constructor
	TicTacToeBoard(std::weak_ptr<Game> game);

	virtual void Initialize() override;
	virtual void UpdateActor(float deltaTime) override;

	// マウスのクリック処理
	void ProcessClick(int x, int y);

private:
	// 特定のタイルを選択
	void SelectTile(int row, int col);

	// 終了判定
	void CheckGameFinished();

private:
	// すべてのタイル
	Array<Array<std::shared_ptr<TicTacToeTile>>> m_tiles;

private:
	// Tileの数
	const int NumRows = 3;
	const int NumCols = 3;

	// 開始Y軸の位置
	const double StartX = 170.0;
	const double StartY = 125.0;

	// タイルのスタート位置
	const int StartTileX = 1, StartTileY = 1;

	// Tileのサイズ
	const double TileSize = 180.0;

	bool m_isFinished = false;
	TicTacToeTile::TileState m_tileState;

	Font m_font{ FontMethod::MSDF, 48 };
};
