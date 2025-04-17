#pragma once
#include <Siv3D.hpp>

void SerializeTest()
{
	// バイナリデータをファイルに保存
	{
		int128 i = 128;
		uint128 j = 256;

		Serializer<BinaryWriter> writer{ U"test.bin" };
		writer(i);
		writer(j);
	}

	// ファイルに保存したバイナリデータを読み込む
	{
		int128 i;
		uint128 j;

		Deserializer<BinaryReader> reader{ U"test.bin" };
		reader(i);
		reader(j);

		Print << i;
		Print << j;
	}

	while (System::Update())
	{

	}
}
