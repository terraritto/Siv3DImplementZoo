#include "MinMaxComponent.h"

Array<Array<GTState>> MinMaxComponent::Decide()
{
	std::shared_ptr<GTNode> choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();

	// Minから探査開始
	for (const auto child : m_root->m_children)
	{
		float v = MinPlayer(child);

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

float MinMaxComponent::MaxPlayer(const std::shared_ptr<GTNode> node)
{
	// 一番下ならスコアを返す
	if (node->m_children.empty())
	{
		return CalculateScore(node->m_state);
	}

	float maxValue = -std::numeric_limits<float>::infinity();
	// 更に下を探索
	for (const auto child : node->m_children)
	{
		maxValue = Math::Max(maxValue, MinPlayer(child));
	}

	return maxValue;
}

float MinMaxComponent::MinPlayer(const std::shared_ptr<GTNode> node)
{
	// 一番下ならスコアを返す
	if (node->m_children.empty())
	{
		return CalculateScore(node->m_state);
	}

	float minValue = std::numeric_limits<float>::infinity();
	// 更に下を探索
	for (const auto child : node->m_children)
	{
		minValue = Math::Min(minValue, MaxPlayer(child));
	}

	return minValue;
}
