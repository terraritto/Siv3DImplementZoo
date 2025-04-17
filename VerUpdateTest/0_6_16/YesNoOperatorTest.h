#pragma once
#include <Siv3D.hpp>

void YesNoOperatorTest()
{
	using YN = YesNo<struct test_Tag>;

	YN Y = YN::Yes;

	Print << (Y == YN::No);

	while (System::Update())
	{
	}
}
