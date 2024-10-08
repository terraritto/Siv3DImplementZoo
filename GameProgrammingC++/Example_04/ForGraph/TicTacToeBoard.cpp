﻿#include "TicTacToeBoard.h"
#include "MinMaxComponent.h"
#include "AlphaBetaComponent.h"

TicTacToeBoard::TicTacToeBoard(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void TicTacToeBoard::Initialize()
{
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// Tileの初期化
	m_tiles.resize(NumRows);
	for (int i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].resize(NumCols);
	}

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			m_tiles[i][j] = std::make_shared<TicTacToeTile>(GetGame());
			m_tiles[i][j]->Initialize();
			m_tiles[i][j]->SetScale(1.5f);
			m_tiles[i][j]->SetPosition(Float2{ StartX + TileSize / 2.0 + j * TileSize, StartY + i * TileSize });
		}
	}

	//m_decide = CreateCastComponent<MinMaxComponent>(this->shared_from_this());
	m_decide = CreateCastComponent<AlphaBetaComponent>(this->shared_from_this());

	this->m_isFinished = false;
}

void TicTacToeBoard::UpdateActor(float deltaTime)
{
	if (this->m_isFinished)
	{
		String winStr;
		switch (this->m_tileState)
		{
		case GTState::Square:
			winStr = U"〇";
			break;
		case GTState::Cross:
			winStr = U"×";
			break;
		default:
			break;
		}

		m_font(U"{}Win"_fmt(winStr)).draw(10, 10);
	}
}

void TicTacToeBoard::ProcessClick(int x, int y)
{
	if (this->m_isFinished) { return; }

	// (0,0)の位置にずらしておく
	x -= static_cast<int>(StartX);
	y -= static_cast<int>(StartY - TileSize / 2.0);

	if (y >= 0)
	{
		// Tileのindexに変換
		x /= static_cast<int>(TileSize);
		y /= static_cast<int>(TileSize);

		// 範囲内にあるか？
		bool isSelectedX = x >= 0 && x < static_cast<int>(NumCols);
		bool isSelectedY = y >= 0 && y < static_cast<int>(NumRows);

		if (isSelectedX && isSelectedY)
		{
			SelectTile(y, x);
		}
	}

	CheckGameFinished();

	if (this->m_isFinished == false)
	{
		auto decide = m_decide.lock();
		decide->Construct(m_tiles);
		auto result = decide->Decide();

		
		if (result.size() != 0)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					m_tiles[i][j]->SetTileState(result[i][j]);
				}
			}
		}

		CheckGameFinished();
	}
}

void TicTacToeBoard::SelectTile(int row, int col)
{
	auto GTState = m_tiles[row][col]->GetTileState();

	if (GTState == GTState::None)
	{
		m_tiles[row][col]->SetTileState(GTState::Square);
	}
}

void TicTacToeBoard::CheckGameFinished()
{
	auto StateCheck = [&](GTState a, GTState b, GTState c)
		{

			if (a == b && b == c && c == a)
			{
				if (a == GTState::None) { return false; }

				this->m_tileState = a;
				return true;
			}
			return false;
		};
	this->m_isFinished |= StateCheck(m_tiles[0][0]->GetTileState(), m_tiles[0][1]->GetTileState(), m_tiles[0][2]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[1][0]->GetTileState(), m_tiles[1][1]->GetTileState(), m_tiles[1][2]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[2][0]->GetTileState(), m_tiles[2][1]->GetTileState(), m_tiles[2][2]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[0][0]->GetTileState(), m_tiles[1][0]->GetTileState(), m_tiles[2][0]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[0][1]->GetTileState(), m_tiles[1][1]->GetTileState(), m_tiles[2][1]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[0][2]->GetTileState(), m_tiles[1][2]->GetTileState(), m_tiles[2][2]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[0][0]->GetTileState(), m_tiles[1][1]->GetTileState(), m_tiles[2][2]->GetTileState());
	this->m_isFinished |= StateCheck(m_tiles[0][2]->GetTileState(), m_tiles[1][1]->GetTileState(), m_tiles[2][0]->GetTileState());
}
