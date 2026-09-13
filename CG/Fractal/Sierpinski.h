#pragma once
#include <Siv3D.hpp>

// シェルピンスキーのGasket
void SierpinskiGasketSample()
{
	struct TriangleVertices
	{
		std::array<Vec2, 3> vertices;
	};

	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// Gasket用集合
	Array<Array<TriangleVertices>> gasketSet;

	// [0,1]時のLineサイズ
	constexpr int MaxSize = 500;

	// 開始位置
	constexpr Vec2 StartPos = { 125, 50 };

	constexpr double BaseY = 50.0;
	constexpr double TriangleHeight = 400.0;

	// 時計回り各Vertexに対して中点を求めればOK
	auto MakeSierpinski = [&](int loop)
	{
		gasketSet.clear();

		TriangleVertices start;
		const double BaseX = Scene::DefaultSceneSize.x / 2.0;
		const double Width = TriangleHeight / Sqrt(3.0);
		start.vertices[0] = Vec2{ BaseX, BaseY };
		start.vertices[1] = Vec2{ BaseX + Width, BaseY + TriangleHeight };
		start.vertices[2] = Vec2{ BaseX - Width, BaseY + TriangleHeight };

		gasketSet.push_back({ start });

		auto current = gasketSet[0];

		for (int i = 0; i < loop; i++)
		{
			Array<TriangleVertices> temp;

			// gasketを構築
			for (int j = 0; j < current.size(); j++)
			{
				auto vertexData = current[j];

				// 頂点構築
				Vec2 edge0Start = vertexData.vertices[0], edge0End = vertexData.vertices[1];
				Vec2 edge1Start = vertexData.vertices[1], edge1End = vertexData.vertices[2];
				Vec2 edge2Start = vertexData.vertices[2], edge2End = vertexData.vertices[0];

				// 中点を求める
				Vec2 medium0Point = (edge0Start + edge0End) / 2.0;
				Vec2 medium1Point = (edge1Start + edge1End) / 2.0;
				Vec2 medium2Point = (edge2Start + edge2End) / 2.0;

				// Triangleを構築
				TriangleVertices divide0Triangle, divide1Triangle, divide2Triangle;
				divide0Triangle.vertices[0] = edge0Start;
				divide0Triangle.vertices[1] = medium0Point;
				divide0Triangle.vertices[2] = medium2Point;

				divide1Triangle.vertices[0] = medium0Point;
				divide1Triangle.vertices[1] = edge1Start;
				divide1Triangle.vertices[2] = medium1Point;

				divide2Triangle.vertices[0] = medium2Point;
				divide2Triangle.vertices[1] = medium1Point;
				divide2Triangle.vertices[2] = edge2Start;

				// 構築した三角形を入れる
				temp.push_back(divide0Triangle);
				temp.push_back(divide1Triangle);
				temp.push_back(divide2Triangle);
			}

			gasketSet.push_back(temp);
			current = temp;
		}
	};

	// 分割数
	int subdivision = 0;

	MakeSierpinski(subdivision);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			subdivision++;
			MakeSierpinski(subdivision);
		}

		// 三角形を描画するだけ
		auto gasketData = gasketSet.back();
		for (int i = 0; i < gasketData.size(); i++)
		{
			auto triangle = gasketData[i];
			Triangle{ triangle.vertices[0], triangle.vertices[1], triangle.vertices[2] }.draw(Palette::Red);
		}
	}
}
