#pragma once
#include <Siv3D.hpp>

void MenuMouseOverTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Array<std::pair<String, Array<String>>> items
	{
		{ U"ゲーム", { U"新規", U"スコア", U"終了" }},
		{ U"ヘルプ", { U"\U000F0625  遊び方", U"\U000F14F7  リリースノート", U"ライセンス" } },
	};

	SimpleMenuBar menuBar{ items };

	while (System::Update())
	{
		ClearPrint();

		bool isMouseOver = menuBar.mouseOver();
		Print << (isMouseOver ? U"Over" : U"None");

		if (const auto item = menuBar.update())
		{
			// 「ゲーム > 終了」が押されたら
			if (item == MenuBarItemIndex{ 0, 2 })
			{
				System::Exit();
			}

			// 「ヘルプ > ライセンス」が押されたら
			if (item == MenuBarItemIndex{ 1, 2 })
			{
				LicenseManager::ShowInBrowser();
			}
		}

		menuBar.draw();
	}
}
