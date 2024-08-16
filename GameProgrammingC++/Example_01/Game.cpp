#include "Game.h"
#include <Siv3D.hpp>

Game::Game()
	: m_isRunning(true)
	, m_isStart(false)
	, m_paddleDir(0)
{
}

bool Game::initialize()
{
	// 背景の色を設定する
	Scene::SetBackground(ColorF{ 0.0, 0.0, 1.0, 1.0 });

	Size sceneSize = Window::GetState().virtualSize;

	// paddleの初期化
	m_paddlePos.x = 10;
	m_paddlePos.y = sceneSize.y / 2;

	// ballの初期化
	m_ballPos.x = sceneSize.x / 2;
	m_ballPos.y = sceneSize.y / 2;
	m_ballVelocity.x = -100;
	m_ballVelocity.y = -200;

    return true;
}

void Game::runLoop()
{
	while (m_isRunning)
	{
		if (System::Update() == false)
		{
			m_isRunning = false;
			break;
		}

		processInput();
		updateGame();
		generateOutput();
	}
}

void Game::shutdown()
{
	// Siv3D内部で諸々はやってくれる
}

void Game::processInput()
{
	if (KeyEscape.pressed())
	{
		m_isRunning = false;
		return;
	}

	if (KeySpace.pressed())
	{
		m_isStart = true;
	}

	m_paddleDir = 0;
	if (KeyW.pressed())
	{
		m_paddleDir--;
	}

	if (KeyS.pressed())
	{
		m_paddleDir++;
	}
}

void Game::updateGame()
{
	if (!m_isStart)
	{
		return;
	}

	double deltaTime = Scene::DeltaTime();
	Size sceneSize = Window::GetState().virtualSize;

	// paddleの更新
	if (m_paddleDir != 0)
	{
		m_paddlePos.y += static_cast<int>(m_paddleDir * 300 * deltaTime);

		int upRestrict = PaddleH / 2 + Thickness;
		int downRestrict = sceneSize.y - PaddleH / 2 - Thickness;

		if (m_paddlePos.y < upRestrict)
		{
			m_paddlePos.y = upRestrict;
		}
		else if (m_paddlePos.y > downRestrict)
		{
			m_paddlePos.y = downRestrict;
		}
	}

	// ballの更新
	m_ballPos.x += static_cast<int>(m_ballVelocity.x * deltaTime);
	m_ballPos.y += static_cast<int>(m_ballVelocity.y * deltaTime);

	// paddleとの交差
	int diff = abs(m_paddlePos.y - m_ballPos.y);
	bool isPaddleIntersect = diff <= PaddleH / 2;

	int ballBoundThreshold = 5;
	bool isExistBallRight = m_ballPos.x <= m_paddlePos.x + Thickness;
	bool isExistBallLeft = m_paddlePos.x + Thickness - ballBoundThreshold <= m_ballPos.x;
	bool isBallExistBound = isExistBallLeft && isExistBallRight;

	if (isPaddleIntersect && isBallExistBound && m_ballVelocity.x < 0)
	{
		m_ballVelocity *= -1;
	}

	// 外に出たので終了
	if (m_ballPos.x < 0)
	{
		m_isRunning = false;
	}

	// 右側の境界チェック
	bool isBallRightBound = m_ballPos.x >= sceneSize.x - Thickness;
	if (isBallRightBound && m_ballVelocity.x > 0)
	{
		
		m_ballVelocity *= -1;
	}

	// ballの上下の制限
	bool isBallUpBound = m_ballPos.y <= Thickness;
	if (isBallUpBound && m_ballVelocity.y < 0)
	{
		m_ballVelocity.y *= -1;
	}

	bool isBallDownBound = sceneSize.y - Thickness <= m_ballPos.y;
	if (isBallDownBound && m_ballVelocity.y > 0)
	{
		m_ballVelocity.y *= -1;
	}
}

void Game::generateOutput()
{
	Size sceneSize = Window::GetState().virtualSize;

	// 壁の生成
	Rect{ 0, 0, sceneSize.x, Thickness }.draw(Palette::White); // Top
	Rect{ 0, sceneSize.y - Thickness, sceneSize.x, Thickness }.draw(Palette::White); // bottom
	Rect{ sceneSize.x - Thickness, 0, Thickness, sceneSize.y }.draw(Palette::White); // right

	// paddleの作成
	Rect{ m_paddlePos.x, m_paddlePos.y - PaddleH / 2, Thickness, PaddleH }.draw(Palette::White);

	// ballの作成
	Circle{ m_ballPos.x, m_ballPos.y, Thickness }.draw(Palette::White);
}
