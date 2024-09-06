#include "AlphaBetaComponent.h"

Array<Array<GTState>> AlphaBetaComponent::Decide()
{
	std::shared_ptr<GTNode> choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();

	// Minから探査開始
	for (const auto child : m_root->m_children)
	{
		float v = MinPlayer(child, maxValue, beta);

		// 最大の値の場合を更新
		if (v > maxValue)
		{
			maxValue = v;
			choice = child;
		}
	}

	Array<Array<GTState>> result;
	result.resize(3);
	for (int i = 0; i < 3; i++)
	{
		result[i].resize(3);
	}

	if (choice == nullptr) { return result; }

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = choice->m_state.m_board[i][j];
		}
	}

	return result;
}

float AlphaBetaComponent::MinPlayer(const std::shared_ptr<GTNode> node, float alpha, float beta)
{
	if (node->m_children.empty())
	{
		return CalculateScore(node->m_state);
	}

	float minValue = std::numeric_limits<float>::infinity();
	// 更に下を探索
	for (const auto child : node->m_children)
	{
		minValue = Math::Min(minValue, MaxPlayer(child, alpha, beta));
		if (minValue <= alpha)
		{
			return minValue; // 枝切り
		}
		beta = Math::Min(minValue, beta);
	}

	return minValue;
}

float AlphaBetaComponent::MaxPlayer(const std::shared_ptr<GTNode> node, float alpha, float beta)
{
	if (node->m_children.empty())
	{
		return CalculateScore(node->m_state);
	}

	float maxValue = -std::numeric_limits<float>::infinity();
	// 更に下を探索
	for (const auto child : node->m_children)
	{
		maxValue = Math::Max(maxValue, MinPlayer(child, alpha, beta));
		if (maxValue >= beta)
		{
			return maxValue; // 枝切り
		}
		alpha = Math::Max(maxValue, alpha);
	}

	return maxValue;
}
