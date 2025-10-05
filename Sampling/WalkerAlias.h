#pragma once
#include <Siv3D.hpp>

void WalkerAliasSample()
{
	struct Data
	{
		int weight;
		Vec2 pos;
		double pdf;
		int index;

		void draw() const
		{
			pos.asCircle(30).draw(ColorF{ 0.95 }).drawFrame(2, ColorF{ 0.11 });
		}

		void drawLabel(const Font& font) const
		{
			font(Format(index)).drawAt(40, pos, ColorF{ 0.11 });
		}

		void drawProbability(const Font& font) const
		{
			font(Format(pdf)).drawAt(40, pos + Vec2::Right() * 150, ColorF{ 0.11 });
		}

		void drawBox(const Font& font, Array<double> p, Array<int> i) const
		{
			Vec2 left = pos + Vec2::Right() * 300;

			constexpr double RectX = 300.0;
			constexpr double RectY = 30.0;
			constexpr double FontOffsetX = 20.0;

			double pSmall = Clamp(p[index], 0.0, 1.0);
			double pLarge = Clamp(1.0 - pSmall, 0.0, 1.0);

			int smallIndex = index;
			int largeIndex = i[index];

			// Smallを描画
			RectF{ left , Vec2{ RectX * pSmall, RectY } }.draw(ColorF{ 0.0,1.0,0.0 });
			font(Format(smallIndex)).drawAt(30, left + Vec2{-FontOffsetX , RectY / 2}, ColorF{ 0.11 });

			// Largeを描画
			RectF{ left + Vec2::Right() * pSmall * RectX, Vec2{RectX * pLarge, RectY} }.draw(ColorF{ 1.0,0.0,0.0 });
			font(Format(largeIndex)).drawAt(30, left + Vec2{ RectX + FontOffsetX, RectY / 2 }, ColorF{ 0.11 });
		}
	};

	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Font font{ FontMethod::MSDF, 36, Typeface::Bold };
	const Font fontp{ FontMethod::MSDF, 40, Typeface::Bold };

	constexpr int DataNum = 5;
	constexpr Vec2 StartPos = Vec2{ 100.0, 50.0 };
	constexpr Vec2 Offset = Vec2{ 0.0, 60.0 };
	Array<Data> dataList;

	// 結果を可視化するために保持する(別段intでなくても良い)
	double sum = 0.0;
	Array<double> probList;

	for (auto index : step(DataNum))
	{
		int weight = index + 1;
		sum += weight;
		dataList.push_back(Data{ weight, StartPos + Offset * index, 0.0, index });

		// 数字を入れる
		probList.push_back(weight);
	}

	// 確率に直す(例:1->1/15,5->5/15的な)
	for (auto& prob : probList)
	{
		prob /= sum;
	}

	// walker's Aliasの構築
	int size = dataList.size();
	double probAve = 1.0 / static_cast<double>(size); // 平均値算出

	// 平均値で割ることで平均の場合1.0の値となるようにする
	Array<double> v = probList;
	std::transform(probList.begin(), probList.end(),
		v.begin(), [&](double a) { return a / probAve; });

	// 平均値を起点に大小を決める
	std::vector<int> small, large, indexList;
	indexList.resize(size);
	for (int i = 0; i < size; i++)
	{
		auto& box = v[i] < 1.0 ? small : large;
		box.push_back(i);
		indexList[i] = i;
	}

	while (small.size() && large.size())
	{
		int j = small.back(); small.pop_back();
		int k = large.back();

		// 小さい方のindexにlargeのindexを割り当てる
		// (小さい箱に大きいものを分割して詰めてるイメージ)
		indexList[j] = k;

		// 1.0の箱の残りにv[k]詰めたので、その分を引く
		v[k] = v[k] - (1.0 - v[j]);

		// largeが1.0になっていたらsmallに移す
		if (v[k] < 1.0)
		{
			small.push_back(k);
			large.pop_back();
		}
	}

	// 箱から適切なindexを選ぶ処理
	Array<int> resultList;
	auto Search = [&]()
	{
		int ri = Random(0, size - 1); // index用乱数
		double r = Random(0.0, 1.0); // 確率用乱数

		// 選ばれた乱数
		int index = 0;

		// 小さい方の確率と判定
		if (r < v[ri])
		{
			// smallそのまま
			index = ri;
		}
		else
		{
			// largeから持ってきた方を選択
			index = indexList[ri];
		}

		resultList[index]++;
	};

	// 確率の可視化に使うサイズはデータの数と同値
	resultList.resize(dataList.size(), 0.0);

	while (System::Update())
	{
		// ボタンが押されると10000回探索が行われる
		if (SimpleGUI::Button(U"10000 sample", Vec2{ 100, 450 }))
		{
			for (auto i : step(10000)) { Search(); }
		}

		// 合計を計算
		double sumTemp = std::reduce(resultList.begin(), resultList.end());

		for (auto i : step(dataList.size()))
		{
			auto& d = dataList[i];

			// indexに対する確率を更新
			d.pdf = sumTemp == 0.0 ? 0.0 : resultList[i] / sumTemp;

			// 描画
			d.draw();
			d.drawLabel(font);
			d.drawProbability(fontp);
			d.drawBox(font, v, indexList);
		}
	}
}
