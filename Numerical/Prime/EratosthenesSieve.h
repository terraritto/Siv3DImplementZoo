#pragma once
#include <Siv3D.hpp>

// エラトステネスの篩
void EratosthenesSieveSample()
{
	Scene::SetBackground(Palette::Black);

	constexpr int N = 1e+3;

	Array<bool> container(N, true);

	// for (int i = 2; i <= Sqrt(N); i++)
	for (int i = 2; i*i <= N; i++)
	{
		if (container[i]) // 素数のみ篩をかける
		{
			// 倍数部分のみ回ればOK
			for (int j = i * i; j < N; j+=i)
			{
				container[j] = false;
			}
		}
	}

	int count = 0;
	String s;
	for (int i = 2; i < N; i++)
	{		
		if (container[i])
		{
			count++;
			s += Format(i) + U",";
		}

		if (count == 10)
		{
			count = 0;
			s += U"\n";
		}
	}
	Print << s;

	while (System::Update())
	{
	}
}
