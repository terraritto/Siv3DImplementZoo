#pragma once
#include <Siv3D.hpp>

class Game
{
public:
	Game();

	bool initialize();
	void runLoop();
	void shutdown();

private:
	void processInput();
	void updateGame();
	void generateOutput();

	bool m_isRunning;
	bool m_isStart;

	int m_paddleDir;
	Vector2D<int> m_paddlePos;
	Vector2D<int> m_ballPos;
	Vector2D<int> m_ballVelocity;

	static constexpr int Thickness = 15;
	static constexpr int PaddleH = 40;
};
