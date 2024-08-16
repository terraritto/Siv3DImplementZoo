#pragma once
#include "GraphBaseComponent.h"
#include "SearchTile.h"
#include "Graph.h"

class AStarComponent : public GraphBaseComponent
{
public:
	AStarComponent(std::weak_ptr<Actor> owner, int drawOrder = 100);

	// override from GraphBaseComponent
	virtual void Construct(Array<Array<std::shared_ptr<SearchTile>>> tiles) override;
	virtual void Calculate() override;

protected:
	double ComputeHeuristic(std::weak_ptr<WeightedGraphNode> target) const;

protected:
	std::unique_ptr<WeightedGraph> m_graph;
	std::shared_ptr<WeightedGraphNode> m_start, m_end;

	std::unordered_map<std::shared_ptr<WeightedGraphNode>, std::shared_ptr<AStarScratch>> m_nodeMap;
};
