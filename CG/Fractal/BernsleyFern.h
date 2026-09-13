#pragma once
#include <Siv3D.hpp>

// バーンズリーのシダ
void BernsleyFernSample()
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	struct TransformFern
	{
		Vec4 m_matrix;
		Vec2 m_translation;
	};

	std::array<TransformFern, 4> Transform;
	// シダの茎
	Transform[0] = TransformFern{ Vec4{0.0,0.0,0.0,0.16}, Vec2{0.0, 0.0} };
	// 連続する小さい葉
	Transform[1] = TransformFern{ Vec4{0.85,0.04,-0.04,0.85}, Vec2{0.0, 1.6} };
	// 左側の大きな葉
	Transform[2] = TransformFern{ Vec4{0.2,-0.26,0.23,0.22}, Vec2{0.0, 1.6} };
	// 右側の大きな葉
	Transform[3] = TransformFern{ Vec4{-0.15,0.28,0.26,0.24}, Vec2{0.0, 0.44} };

	Array<double> probabilities{ 0.01,0.85,0.07,0.07 };
	std::random_device seed_gen;
	std::uint32_t seed = seed_gen();
	std::mt19937 engine(seed);
	std::discrete_distribution<std::size_t> dist(
	  probabilities.begin(),
	  probabilities.end()
	);

	Array<Vec2> pointSet;

	auto MakeFern = [&](int loop)
	{
		pointSet.clear();
		Vec2 point = Vec2::Zero();

		for (int i = 0; i < loop; i++)
		{
			auto index = dist(engine);

			// 位置を計算
			double tempX =
				point.x * Transform[index].m_matrix.x +
				point.y * Transform[index].m_matrix.y +
				Transform[index].m_translation.x;

			double tempY =
				point.x * Transform[index].m_matrix.z +
				point.y * Transform[index].m_matrix.w +
				Transform[index].m_translation.y;
			point = { tempX,tempY };

			// 追加
			pointSet.push_back(point);
		}
	};

	// [0,1]時のLineサイズ
	constexpr int MaxSize = 50;

	// 開始位置
	const Vec2 StartPos = Vec2{ Scene::Center().x, 500.0 };

	// ループ数
	int loop = 1000;

	MakeFern(loop);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"サンプル追加", Vec2{ 10, 10 }))
		{
			loop += 1000;
			MakeFern(loop);
		}

		for (auto pp : pointSet)
		{
			Vec2 center = StartPos + pp.withY(pp.y * -1.0) * MaxSize;
			Circle{ center, 1.0 }.draw(Palette::Red);
		}
	}
}
