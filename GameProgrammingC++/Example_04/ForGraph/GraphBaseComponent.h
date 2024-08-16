#pragma once
#include "../Component.h"
#include "SearchTile.h"

class GraphBaseComponent : public Component
{
public:
	using Component::Component;

	// 計算に必要なものを生成
	virtual void Construct(Array<Array<std::shared_ptr<SearchTile>>>) = 0;

	// 計算してTile内の情報を決定
	virtual void Calculate() = 0;
};
