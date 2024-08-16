#pragma once
#include "GraphBaseComponent.h"
#include "SearchTile.h"
#include "Graph.h"

class BFSComponent : public GraphBaseComponent
{
public:
	BFSComponent(std::weak_ptr<Actor> owner, int drawOrder = 100);

	// override from GraphBaseComponent
	virtual void Construct(Array<Array<std::shared_ptr<SearchTile>>> tiles) override;
	virtual void Calculate() override;

protected:
	std::unique_ptr<Graph> m_graph;
	std::shared_ptr<GraphNode> m_start, m_end;

	std::unordered_map<std::shared_ptr<GraphNode>, std::shared_ptr<GraphNode>> m_nodeMap;
};
