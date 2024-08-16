# include <Siv3D.hpp>
#include "Example_04/Game.h"

void Main()
{
	std::shared_ptr<Game> game = std::make_shared<Game>();

	// 初期化
	bool success = game->initialize();

	if (success)
	{
		// ゲームループ
		game->runLoop();
	}

	// 終了
	game->shutdown();
}
