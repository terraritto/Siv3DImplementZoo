#pragma once
#include <Siv3D.hpp>

void StringChoiceTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	String Test = U"ABCDEFGHIJKLMN";
	auto RandString = Test.choice();

	const Font font{ 50 };

	while (System::Update())
	{
		font(Test).draw(100, 100);
		font(RandString).draw(100, 200);
	}
}
