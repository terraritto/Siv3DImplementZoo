#include "SearchGrid.h"
#include "../Component.h"
#include "SearchTile.h"
#include "BFSComponent.h"
#include "DFSComponent.h"
#include "GBFSComponent.h"
#include "AStarComponent.h"
#include "DijkstraComponent.h"
#include "WeightedAStarComponent.h"

SearchGrid::SearchGrid(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void SearchGrid::Initialize()
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
			m_tiles[i][j] = std::make_shared<SearchTile>(GetGame());
			m_tiles[i][j]->Initialize();
			m_tiles[i][j]->SetScale(0.1);
			m_tiles[i][j]->SetPosition(Float2{ StartX + TileSize / 2.0 + j * TileSize, StartY + i * TileSize });
		}
	}
}

void SearchGrid::UpdateActor(float deltaTime)
{
	const int Offset = 50;
	
	// 掘る用ボタン
	if (SimpleGUI::Button(U"Dig", { 10, 10 }))
	{
		MakeGrid();
	}

	// 終了位置決定ボタン
	if (SimpleGUI::Button(U"SetEnd", { 10, 10 + Offset }))
	{
		MakeEndPoint();
	}

	// 壁貫通ボタン
	if (SimpleGUI::Button(U"Penetrate", { 10, 10 + Offset * 2 }))
	{
		WallPenetrate();
	}

	// 壁貫通ボタン
	if (SimpleGUI::Button(U"Clear", { 10, 10 + Offset * 3 }))
	{
		WallClear();
	}

	// 確率
	SimpleGUI::Slider(m_weight, 0, 3, { 10, 10 + Offset * 4 });

	// Endが追加されていない場合は何もしない
	if (IsEndExist() == false)
	{
		return;
	}

	if (SimpleGUI::Button(U"Dijkstra!!", { 10, 10 + Offset * 5 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<DijkstraComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	if (SimpleGUI::Button(U"Weighted A*", { 10, 10 + Offset * 6 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		auto component = CreateCastComponent<WeightedAStarComponent>(owner).lock();
		component->SetWeight(m_weight);
		m_graphComponent = component;
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	int count = 0;
	for (auto& tiles : m_tiles)
	{
		for (auto& tile : tiles)
		{
			if (SearchTile::TileState::FindPath == tile->GetTileState())
			{
				count++;
			}
		}
	}


	PutText(U"PathCount: " + Format(count), {30, 10 + Offset * 7});

	/*
	// 探索開始
	if (SimpleGUI::Button(U"BFS!!", { 10, 10 + Offset * 4 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<BFSComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	if (SimpleGUI::Button(U"DFS!!", { 10, 10 + Offset * 5 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<DFSComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	if (SimpleGUI::Button(U"GBFS!!", { 10, 10 + Offset * 6 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<GBFSComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	if (SimpleGUI::Button(U"AStar!!", { 10, 10 + Offset * 7 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<AStarComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}

	if (SimpleGUI::Button(U"Dijkstra!!", { 10, 10 + Offset * 8 }))
	{
		auto game = m_game.lock();
		auto owner = this->shared_from_this();
		m_graphComponent = CreateCastComponent<DijkstraComponent>(owner).lock();
		m_graphComponent->Construct(m_tiles);
		m_graphComponent->Calculate();
	}
	*/
}

void SearchGrid::MakeGrid()
{
	// 状態を全部初期化
	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			m_tiles[i][j]->SetTileState(SearchTile::TileState::Wall);
		}
	}

	// 外壁をdefaultに設定しておく
	SetupOuterState(SearchTile::TileState::Default);

	// 穴掘り法を適用
	Dig(StartTileX, StartTileY);

	// スタート位置を確定させる
	GetStartTile().lock()->SetTileState(SearchTile::TileState::Start);

	// 外壁をwallにする
	SetupOuterState(SearchTile::TileState::Wall);
}

void SearchGrid::MakeEndPoint()
{
	Array<std::shared_ptr<SearchTile>> defaultTiles;

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			// defaultは追加
			if (m_tiles[i][j]->GetTileState() == SearchTile::TileState::Default)
			{
				defaultTiles.push_back(m_tiles[i][j]);
			}

			// endはdefaultに戻してから追加
			if (m_tiles[i][j]->GetTileState() == SearchTile::TileState::End)
			{
				defaultTiles.push_back(m_tiles[i][j]);
				m_tiles[i][j]->SetTileState(SearchTile::TileState::Default);
			}
		}
	}

	// ランダムなdefaultを一つ選択してEndにする
	auto tile = defaultTiles[Random<int>(0, defaultTiles.size() - 1)];
	tile->SetTileState(SearchTile::TileState::End);
}

void SearchGrid::WallPenetrate()
{
	Array<std::shared_ptr<SearchTile>> wallTiles;

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			// defaultは追加
			if (m_tiles[i][j]->GetTileState() == SearchTile::TileState::Wall)
			{
				wallTiles.push_back(m_tiles[i][j]);
			}
		}
	}

	// ランダムなdefaultを選択してDefaultにする
	constexpr int PenetrateNum = 100;
	for (int i = 0; i < PenetrateNum; i++)
	{
		auto tile = wallTiles[Random<int>(0, wallTiles.size() - 1)];
		tile->SetTileState(SearchTile::TileState::Default);
	}
}

void SearchGrid::WallClear()
{
	for (auto& tiles : m_tiles)
	{
		for (auto& tile : tiles)
		{
			if (tile->GetTileState() == SearchTile::TileState::Searched)
			{
				tile->SetTileState(SearchTile::TileState::Default);
			}

			switch (tile->GetTileState())
			{
			case SearchTile::TileState::Searched:
			case SearchTile::TileState::FindPath:
				tile->SetTileState(SearchTile::TileState::Default);
				break;
			default:
				break;
			}
		}
	}
}

bool SearchGrid::IsEndExist() const
{
	for (auto& tiles : m_tiles)
	{
		for (auto& tile : tiles)
		{
			if (tile->GetTileState() == SearchTile::TileState::End)
			{
				return true;
			}
		}
	}
	return false;
}

void SearchGrid::Dig(int x, int y)
{
	while (true)
	{
		Array<Direction> directionList;

		// 掘り進められる方向を保存
		// up
		if (1 <= y - 2)
		{
			if (
				m_tiles[x][y - 1]->GetTileState() == SearchTile::TileState::Wall &&
				m_tiles[x][y - 2]->GetTileState() == SearchTile::TileState::Wall
			)
			{
				directionList.push_back(Direction::Up);
			}
		}

		// down
		if (y + 2 <= NumCols - 2)
		{
			if (
				m_tiles[x][y + 1]->GetTileState() == SearchTile::TileState::Wall &&
				m_tiles[x][y + 2]->GetTileState() == SearchTile::TileState::Wall
			)
			{
				directionList.push_back(Direction::Down);
			}
		}

		// left
		if (1 <= x - 2)
		{
			if (
				m_tiles[x - 1][y]->GetTileState() == SearchTile::TileState::Wall &&
				m_tiles[x - 2][y]->GetTileState() == SearchTile::TileState::Wall
			)
			{
				directionList.push_back(Direction::Left);
			}
		}

		// Right
		if (x + 2 <= NumRows - 2)
		{
			if (
				m_tiles[x + 1][y]->GetTileState() == SearchTile::TileState::Wall &&
				m_tiles[x + 2][y]->GetTileState() == SearchTile::TileState::Wall
			)
			{
				directionList.push_back(Direction::Right);
			}
		}

		// directionがないなら終了
		if (directionList.size() == 0) { break; }

		// 現在の位置をPathに変更
		SetPath(x, y);

		// 方向を確定
		const int directionIndex = Random<int>(0, directionList.size() - 1);

		// 決まった方向の2マスを通路に
		switch (directionList[directionIndex])
		{
		case Direction::Up:
			SetPath(x, y - 1);
			SetPath(x, y - 2);
			break;

		case Direction::Down:
			SetPath(x, y + 1);
			SetPath(x, y + 2);
			break;

		case Direction::Left:
			SetPath(x - 1, y);
			SetPath(x - 2, y);
			break;

		case Direction::Right:
			SetPath(x + 1, y);
			SetPath(x + 2, y);
			break;

		default:
			break;
		}
	}

	// スタートがない場合は終了
	if (m_poolCellList.size() == 0) { return; }

	// ランダムに取得して掘る
	const int digIndex = Random<int>(0, m_poolCellList.size() - 1);
	auto [digX, digY] = m_poolCellList[digIndex];
	m_poolCellList.remove_at(digIndex);
	Dig(digX, digY);
}

void SearchGrid::SetPath(int x, int y)
{
	m_tiles[x][y]->SetTileState(SearchTile::TileState::Default);

	if (IsOdd(x) && IsOdd(y))
	{
		m_poolCellList.push_back({ x, y });
	}
}

void SearchGrid::SetupOuterState(SearchTile::TileState state)
{
	// 壁を確定させる
	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			// 一番端を壁に
			if (i == 0 || i == NumRows - 1 ||
				j == 0 || j == NumRows - 1)
			{
				m_tiles[i][j]->SetTileState(state);
			}
		}
	}
}
