#pragma once

# include <Siv3D.hpp>

using VertexID = int;

constexpr int MAXNUM = 1 << 10;

struct Vertex
{
	String name;

	Vec2 pos;

	int weight = MAXNUM;

	void draw() const
	{
		pos.asCircle(40).draw(ColorF{ 0.95 }).drawFrame(2, ColorF{ 0.11 });
	}

	void drawLabel(const Font& font) const
	{
		String weightText = weight == MAXNUM ? U"INF" : Format(weight);
		font(name + U" : " + weightText).drawAt(40, pos, ColorF{0.11});
	}
};

void DrawEdge(const Vertex& from, const Vertex& to, const int weight)
{
	Line{ from.pos, to.pos }.stretched(-40).drawArrow(3, Vec2{ 15, 15 }, weight >= 0 ? Palette::Red : Palette::Blue);
}

void Example_01(Array<Array<std::pair<VertexID, int>>>& graphs, int& vertexNum, int& start)
{
	vertexNum = 4;
	start = 0;
	graphs[0] = { {1,2}, {2,3} };
	graphs[1] = { {2,-5}, {3,1} };
	graphs[2] = { {3,2} };
}

void Example_02(Array<Array<std::pair<VertexID, int>>>& graphs, int& vertexNum, int& start)
{
	vertexNum = 4;
	start = 0;
	graphs[0] = { {1,2}, {2,3} };
	graphs[1] = { {2,-5}, {3,1} };
	graphs[2] = { {3,2} };
	graphs[3] = { {1,0} };
}

void Example_03(Array<Array<std::pair<VertexID, int>>>& graphs, int& vertexNum, int& start)
{
	vertexNum = 4;
	start = 1;
	graphs[0] = { {1,2}, {2,3} };
	graphs[1] = { {2,-5}, {3,1} };
	graphs[2] = { {3,2} };
}


void BellmanFordSample()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Font font{ FontMethod::MSDF, 36, Typeface::Bold };

	Array<Array<std::pair<VertexID, int>>> graphs(6);
	int vertexNum = 0;
	int start = 0;
	Example_01(graphs, vertexNum, start);

	Array<Vertex> vertices;
	for (size_t i = 0; i < graphs.size(); ++i)
	{
		const double rad = (i * (Math::TwoPi / graphs.size()));
		vertices.push_back(Vertex{ Format(i), OffsetCircular{ Scene::Center(), 200, rad } });
	}
	vertices[start].weight = 0;

	// Bellman-Ford法を実行
	Array<Array<Vertex>> memoVertices; // 各ステップの頂点をメモ

	int count = 0;
	while (count < vertexNum)
	{
		memoVertices.push_back(vertices);
		bool end = true;
		for (int index = 0; const auto& graph : graphs)
		{
			for (auto& e : graph)
			{
				int newCost = vertices[index].weight + e.second;
				if	(newCost < vertices[e.first].weight)
				{
					vertices[e.first].weight = newCost;
					end = false;
					memoVertices.push_back(vertices);
				}
			}

			index++;
		}

		if (end) { break; }
		count++;
	}

	const int memoVertexNum = memoVertices.size();
	int page = 0;
	while (System::Update())
	{
		if (SimpleGUI::Button(U"Prev", Vec2{ 50,50 })) { page--; }
		if (SimpleGUI::Button(U"Next", Vec2{ 50,100 })) { page++; }
		page = Clamp(page, 0, memoVertexNum - 1);

		for (const auto& v : memoVertices[page])
		{
			v.draw();
		}

		for (size_t from = 0; from < graphs.size(); ++from)
		{
			for (const auto& to : graphs[from])
			{
				DrawEdge(vertices[from], vertices[to.first], to.second);
			}
		}

		for (const auto& v : memoVertices[page])
		{
			v.drawLabel(font);
		}

		PutText(U"Max: " + Format(memoVertexNum), { 50, 150 });
		PutText(U"page: " + Format(page), { 50, 200 });
	}
}
