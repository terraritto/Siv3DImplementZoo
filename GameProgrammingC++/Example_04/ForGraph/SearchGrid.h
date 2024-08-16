#pragma once
#include "../Actor.h"
#include "SearchTile.h"
#include "GraphBaseComponent.h"
#include <vector>

class SearchGrid : public Actor
{
public:
	// constructor
	SearchGrid(std::weak_ptr<Game> game);

	virtual void Initialize() override;
	virtual void UpdateActor(float deltaTime) override;

	std::weak_ptr<SearchTile> GetStartTile() { return m_tiles[StartTileX][StartTileY]; }

protected:
	// Gridの生成
	void MakeGrid();

	// 終了位置の生成
	void MakeEndPoint();

	// 壁を消す
	void WallPenetrate();

	// 座標(x,y)を掘る
	void Dig(int x, int y);

	// 座標を通路にして開始座標をpoolする
	void SetPath(int x, int y);

	// 外壁を埋める
	void SetupOuterState(SearchTile::TileState state);

private:
	// 方向
	enum class Direction
	{
		Up = 0,
		Right = 1,
		Down = 2,
		Left = 3
	};

private:
	// すべてのタイル
	Array<Array<std::shared_ptr<SearchTile>>> m_tiles;

	// 計算用
	Array<std::pair<int, int>> m_poolCellList;

	// GraphBase用のコンポーネント
	std::shared_ptr<GraphBaseComponent> m_graphComponent;

private:
	// Tileの数
	const int NumRows = 51;
	const int NumCols = 51;

	// 開始Y軸の位置
	const double StartX = 150.0;
	const double StartY = 50.0;

	// タイルのスタート位置
	const int StartTileX = 1, StartTileY = 1;

	// Tileのサイズ
	const double TileSize = 10.0;
};
