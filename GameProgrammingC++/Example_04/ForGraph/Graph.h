#pragma once
#include <Siv3D.hpp>
#include "SearchTile.h"

struct GraphNode
{
	std::weak_ptr<SearchTile> m_tile;
	std::vector<std::shared_ptr<GraphNode>> m_adjacent;
};

struct Graph
{
	std::vector<std::vector<std::shared_ptr<GraphNode>>> m_nodes;
};

struct WeightedEdge
{
	std::weak_ptr<struct WeightedGraphNode> m_from;
	std::weak_ptr<struct WeightedGraphNode> m_to;
	float m_weight;
};

struct WeightedGraphNode
{
	std::weak_ptr<SearchTile> m_tile;
	Vec2 m_index;
	std::vector<std::shared_ptr<WeightedEdge>> m_edges;
};

struct WeightedGraph
{
	std::vector<std::vector<std::shared_ptr<WeightedGraphNode>>> m_nodes;
};

struct GBFSScratch
{
	std::shared_ptr<WeightedEdge> m_parentEdge;
	float m_heuristic = 0.0f;
	bool m_isOpenSet = false;
	bool m_isClosedSet = false;
};

struct AStarScratch
{
	std::shared_ptr<WeightedEdge> m_parentEdge;
	float m_heuristic = 0.0f;
	float m_actualFromStart = 0.0f;
	bool m_isOpenSet = false;
	bool m_isClosedSet = false;
};

struct DijkstraScratch
{
	std::shared_ptr<WeightedEdge> m_parentEdge;
	float m_actualFromStart = 0.0f;
	bool m_isOpenSet = false;
	bool m_isClosedSet = false;
};
