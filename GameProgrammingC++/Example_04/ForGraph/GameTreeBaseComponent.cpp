#include "GameTreeBaseComponent.h"

void GameTreeBaseComponent::Construct(Array<Array<std::shared_ptr<TicTacToeTile>>> base)
{
	m_root.reset();
	m_root = std::make_shared<GTNode>();
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_root->m_state.m_board[i][j] = base[i][j]->GetTileState();
		}
	}

	// xからスタート
	GenerateStates(m_root, true);
}

void GameTreeBaseComponent::GenerateStates(std::shared_ptr<GTNode> root, bool xPlayer)
{
	int score = CalculateScore(root->m_state);

	// 決着がついてるので打ち切り
	if (score != 0)
	{
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{			
			if (root->m_state.m_board[i][j] == GTState::None)
			{
				// 交互に生成することで全状態を生成
				auto node = std::make_shared<GTNode>();
				root->m_children.emplace_back(node);
				node->m_state = root->m_state;
				node->m_state.m_board[i][j] = xPlayer ? GTState::Cross : GTState::Square;
				GenerateStates(node, !xPlayer);
			}
		}
	}
}

float GameTreeBaseComponent::CalculateScore(const GTBoard& board)
{
	GTState state = GTState::None;

	auto StateCheck = [&](GTState a, GTState b, GTState c)
		{

			if (a == b && b == c && c == a)
			{
				if (a == GTState::None) { return; }

				state = a;
			}
			return;
		};

	StateCheck(board.m_board[0][0], board.m_board[0][1], board.m_board[0][2]);
	StateCheck(board.m_board[1][0], board.m_board[1][1], board.m_board[1][2]);
	StateCheck(board.m_board[2][0], board.m_board[2][1], board.m_board[2][2]);
	StateCheck(board.m_board[0][0], board.m_board[1][0], board.m_board[2][0]);
	StateCheck(board.m_board[0][1], board.m_board[1][1], board.m_board[2][1]);
	StateCheck(board.m_board[0][2], board.m_board[1][2], board.m_board[2][2]);
	StateCheck(board.m_board[0][0], board.m_board[1][1], board.m_board[2][2]);
	StateCheck(board.m_board[0][2], board.m_board[1][1], board.m_board[2][0]);

	switch (state)
	{
	// 勝ち
	case GTState::Cross:
		return 1.0f;

	// 負け
	case GTState::Square:
		return -1.0f;

	// あいこ or 確定しない
	default:
		return 0.0f;
	}
}
