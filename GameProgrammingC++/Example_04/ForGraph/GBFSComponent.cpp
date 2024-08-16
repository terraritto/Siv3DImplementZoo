﻿#include "GBFSComponent.h"

GBFSComponent::GBFSComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: GraphBaseComponent(owner, drawOrder)
{
}

void GBFSComponent::Construct(Array<Array<std::shared_ptr<SearchTile>>> tiles)
{
	// graphをサイズ分初期化
	m_graph = std::make_unique<WeightedGraph>();
	m_graph->m_nodes.resize(tiles.size());
	for (int i = 0; i < tiles.size(); i++)
	{
		m_graph->m_nodes[i].resize(tiles[0].size());
	}

	// graphをtileで生成
	for (int i = 0; i < tiles.size(); i++)
	{
		for (int j = 0; j < tiles[0].size(); j++)
		{
			m_graph->m_nodes[i][j] = std::make_shared<WeightedGraphNode>();
			m_graph->m_nodes[i][j]->m_tile = tiles[i][j];
			m_graph->m_nodes[i][j]->m_index = Vec2{ i,j };

			if (tiles[i][j]->GetTileState() == SearchTile::TileState::Start)
			{
				m_start = m_graph->m_nodes[i][j];
			}

			// start,endを保持
			if (tiles[i][j]->GetTileState() == SearchTile::TileState::End)
			{
				m_end = m_graph->m_nodes[i][j];
			}
		}
	}

	// adjustmentを解決
	for (int i = 0; i < tiles.size(); i++)
	{
		for (int j = 0; j < tiles[0].size(); j++)
		{
			auto node = m_graph->m_nodes[i][j];

			// 四方向を突っ込む
			if (i - 1 > 0)
			{
				auto edge = std::make_shared<WeightedEdge>();
				edge->m_from = node;
				edge->m_to = m_graph->m_nodes[i - 1][j];
				edge->m_weight = 1.0f;
				node->m_edges.emplace_back(edge);
			}
			if (j - 1 > 0)
			{
				auto edge = std::make_shared<WeightedEdge>();
				edge->m_from = node;
				edge->m_to = m_graph->m_nodes[i][j - 1];
				edge->m_weight = 1.0f;
				node->m_edges.emplace_back(edge);
			}
			if (i + 1 < tiles[0].size() - 1) {
				auto edge = std::make_shared<WeightedEdge>();
				edge->m_from = node;
				edge->m_to = m_graph->m_nodes[i + 1][j];
				edge->m_weight = 1.0f;
				node->m_edges.emplace_back(edge);
			}
			if (j + 1 < tiles[0].size() - 1)
			{
				auto edge = std::make_shared<WeightedEdge>();
				edge->m_from = node;
				edge->m_to = m_graph->m_nodes[i][j + 1];
				edge->m_weight = 1.0f;
				node->m_edges.emplace_back(edge);
			}
		}
	}
}

void GBFSComponent::Calculate()
{
	std::vector<std::shared_ptr<WeightedGraphNode>> openSet;

	// スタートを最初のノードとし、closeしておく
	std::shared_ptr<WeightedGraphNode> current = m_start;
	std::shared_ptr<GBFSScratch> scratch = std::make_shared<GBFSScratch>();
	scratch->m_isClosedSet = true;
	m_nodeMap[current] = scratch;

	do
	{
		// 隣接のノードをOpenをしていく
		for (const std::shared_ptr<WeightedEdge> edge : current->m_edges)
		{
			auto pEdgeTo = edge->m_to.lock();

			if (pEdgeTo->m_tile.lock()->GetTileState() != SearchTile::TileState::Default &&
				pEdgeTo->m_tile.lock()->GetTileState() != SearchTile::TileState::End)
			{
				continue;
			}

			// データを取得
			auto data = m_nodeMap[pEdgeTo];

			// そもそもない場合は生成
			if (data == nullptr)
			{
				m_nodeMap[pEdgeTo] = std::make_shared<GBFSScratch>();
				data = m_nodeMap[pEdgeTo];
			}

			// Closeでない時に限り追加
			if (data->m_isClosedSet == false)
			{
				// 隣接ノードの親エッジに設定
				data->m_parentEdge = edge;

				// Openでないならheuristicを計算
				if (data->m_isOpenSet == false)
				{
					// heuristicを計算し、Openに設定
					data->m_heuristic = ComputeHeuristic(edge->m_to);
					data->m_isOpenSet = true;
					openSet.emplace_back(edge->m_to);
				}
			}
		}

		// open集合がないなら、全てのPathを探索完了
		if (openSet.empty())
		{
			break;
		}

		// 低いコストをOpenから見つける
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&](const std::shared_ptr<WeightedGraphNode> a, const std::shared_ptr<WeightedGraphNode> b)
			{
				return m_nodeMap[a]->m_heuristic < m_nodeMap[b]->m_heuristic;
			}
		);

		// currentに低コストのものをセットした後、
		// openからcloseにする
		current = *iter;
		openSet.erase(iter);
		m_nodeMap[current]->m_isOpenSet = false;
		m_nodeMap[current]->m_isClosedSet = true;
	} while (current != m_end);

	// 探索済みにマーク
	auto NodeChangeSearchedState = [](std::shared_ptr<WeightedGraphNode> node)
		{
			if (node == nullptr) { return; }

			auto pTile = node->m_tile.lock();

			if (
				pTile->GetTileState() == SearchTile::TileState::End ||
				pTile->GetTileState() == SearchTile::TileState::Start ||
				pTile->GetTileState() == SearchTile::TileState::Wall
				)
			{
				return;
			}

			pTile->SetTileState(SearchTile::TileState::Searched);
	};

	for (auto& node : m_nodeMap)
	{
		if (node.second == nullptr || node.second->m_parentEdge == nullptr) { continue; }
		NodeChangeSearchedState(node.second->m_parentEdge->m_from.lock());
		NodeChangeSearchedState(node.second->m_parentEdge->m_to.lock());
	}

	// 見つかったパスにマーク
	std::shared_ptr<WeightedGraphNode> result = m_end;
	while (true)
	{
		auto next = m_nodeMap[result];
		if (next == nullptr)
		{
			break;
		}

		auto pFrom = next->m_parentEdge->m_from.lock();
		if (pFrom == m_start) { break; }
		pFrom->m_tile.lock()->SetTileState(SearchTile::TileState::FindPath);
		result = pFrom;
	}
}

double GBFSComponent::ComputeHeuristic(std::weak_ptr<WeightedGraphNode> target) const
{
	auto pTarget = target.lock();
	auto pEnd = m_end;

	// マンハッタン距離を計算
	Vec2 d = pEnd->m_index - pTarget->m_index;
	return abs(d.x) + abs(d.y);
}
