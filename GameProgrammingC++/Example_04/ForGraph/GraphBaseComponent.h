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

	// 確率を設定
	void SetMinusProbability(const double prob) { this->m_minusProbability = prob; }

protected:
	// 重みを取得
	[[nodiscard]]
	double GetRandomWeight() const
	{
		double value = Random(0.0, 5.0);
		return m_minusProbability <= Random() ? value : -value;
	}

protected:
	double m_minusProbability = 0.0;
};
