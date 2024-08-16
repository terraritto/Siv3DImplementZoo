#include "BFSComponent.h"

BFSComponent::BFSComponent(std::weak_ptr<Actor> owner, int drawOrder)
	: GraphBaseComponent(owner, drawOrder)
{
}

void BFSComponent::Construct(Array<Array<std::shared_ptr<SearchTile>>> tiles)
{
	// graphをサイズ分初期化
	m_graph = std::make_unique<Graph>();
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
			m_graph->m_nodes[i][j] = std::make_shared<GraphNode>();
			m_graph->m_nodes[i][j]->m_tile = tiles[i][j];

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
			if (i - 1 > 0) { node->m_adjacent.push_back(m_graph->m_nodes[i-1][j]); }
			if (j - 1 > 0) { node->m_adjacent.push_back(m_graph->m_nodes[i][j-1]); }
			if (i + 1 < tiles[0].size() - 1) { node->m_adjacent.push_back(m_graph->m_nodes[i+1][j]); }
			if (j + 1 < tiles[0].size() - 1) { node->m_adjacent.push_back(m_graph->m_nodes[i][j+1]); }
		}
	}
}

void BFSComponent::Calculate()
{
	m_nodeMap.clear();

	// 辿っていくqueue
	std::queue<std::shared_ptr<GraphNode>> q;
	q.emplace(m_start);

	// 探るものがなくなるまで入れる
	while (!q.empty())
	{
		auto current = q.front(); q.pop();

		if (current == m_end)
		{
			break;
		}

		for (auto node : current->m_adjacent)
		{
			auto parent = m_nodeMap[node];
			if (parent == nullptr && node != m_start)
			{
				// 進むのはdefaultとendのみ許可
				if (
					node->m_tile.lock()->GetTileState() != SearchTile::TileState::Default &&
					node->m_tile.lock()->GetTileState() != SearchTile::TileState::End
					)
				{
					continue;
				}

				m_nodeMap[node] = current;
				q.emplace(node);
			}
		}
	}

	// 探索済みにマーク
	auto NodeChangeSearchedState = [](std::shared_ptr<GraphNode> node)
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
		NodeChangeSearchedState(node.first);
		NodeChangeSearchedState(node.second);
	}

	// 見つかったパスにマーク
	std::shared_ptr<GraphNode> current = m_end;
	while (true)
	{
		auto next = m_nodeMap[current];
		if (next == nullptr)
		{
			break;
		}

		if (next == m_start) { break; }
		next->m_tile.lock()->SetTileState(SearchTile::TileState::FindPath);
		current = next;
	}
}
