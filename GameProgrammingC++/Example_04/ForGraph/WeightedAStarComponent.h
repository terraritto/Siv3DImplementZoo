#pragma once
#include "GraphBaseComponent.h"
#include "SearchTile.h"
#include "Graph.h"

class WeightedAStarComponent : public GraphBaseComponent
{
public:
	WeightedAStarComponent(std::weak_ptr<Actor> owner, int drawOrder = 100);

	// override from GraphBaseComponent
	virtual void Construct(Array<Array<std::shared_ptr<SearchTile>>> tiles) override;
	virtual void Calculate() override;

	void SetWeight(const double weight) { m_weight = weight; }

protected:
	double ComputeHeuristic(std::weak_ptr<WeightedGraphNode> target) const;

protected:
	std::unique_ptr<WeightedGraph> m_graph;
	std::shared_ptr<WeightedGraphNode> m_start, m_end;

	std::unordered_map<std::shared_ptr<WeightedGraphNode>, std::shared_ptr<AStarScratch>> m_nodeMap;

	double m_weight = 1.0;
};
