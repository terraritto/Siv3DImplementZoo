#pragma once
#include "../Component.h"
#include "TicTacToetile.h"

class GameTreeBaseComponent : public Component
{
public:
	using Component::Component;

	// 構築
	void Construct(Array<Array<std::shared_ptr<TicTacToeTile>>> base);

	// 決定
	virtual Array<Array<GTState>> Decide() = 0;

protected:
	// 全状態の生成
	void GenerateStates(std::shared_ptr<GTNode> root, bool xPlayer);

	// Scoreの計算
	float CalculateScore(const GTBoard& board);

protected:
	std::shared_ptr<GTNode> m_root;
};
