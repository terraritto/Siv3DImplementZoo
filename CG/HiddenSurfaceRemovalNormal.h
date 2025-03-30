#pragma once

#pragma once
#include "Siv3D.hpp"

void LoadObj(String filePath, Array<Vec4>& vertices, Array<Vector3D<int>>& indices)
{
	TextReader reader{ filePath };

	if (reader)
	{
		String line;
		while (reader.readLine(line))
		{
			if (not line) { continue; }
			const Array<String> items = line.split(U' ');

			if (items[0] == U"v")
			{
				vertices.push_back(Vec4{ Parse<double>(items[1]), Parse<double>(items[2]), Parse<double>(items[3]), 1.0 });
				continue;
			}

			if (items[0] == U"f")
			{
				// indexが1始まりなので、0始まりにするために-1しておく
				if (items[1].contains(U'/'))
				{
					indices.push_back
					(
						{
							Parse<int>(items[1].split(U'/')[0]) - 1,
							Parse<int>(items[2].split(U'/')[0]) - 1,
							Parse<int>(items[3].split(U'/')[0]) - 1
						}
					);
				}
				else
				{
					indices.push_back
					(
						{
							Parse<int>(items[1]) - 1,
							Parse<int>(items[2]) - 1,
							Parse<int>(items[3]) - 1
						}
					);
				}
				continue;
			}
		}
	}
}

void BresenhamAll(Vec2 p1, Vec2 p2, Image& image)
{
	int dx = Round(p2.x - p1.x);
	int dy = Round(p2.y - p1.y);

	if (dx == 0 && dy == 0) { return; }

	Vector2D<int> p2Int({ static_cast<int>(Round(p2.x)), static_cast<int>(Round(p2.y)) });

	int diffX = dx >= 0 ? 1 : -1;
	int diffY = dy >= 0 ? 1 : -1;

	// プラスにしておく
	dx = abs(dx);
	dy = abs(dy);

	int x = Round(p1.x), y = Round(p1.y);
	int d = diffX >= diffY ? 2 * dy - dx : 2 * dx - dy;
	d = diffX == diffY ? -1 : d; // 0の場合は-1にして動かないようにする
	image[y][x] = Palette::White;

	if (dx >= dy)
	{
		while (x != p2Int.x)
		{
			x += diffX;
			if (d >= 0)
			{
				y += diffY;
				d -= 2 * dx;
			}
			d += 2 * dy;
			image[y][x] = Palette::White;
		}
	}
	else
	{
		while (y != p2Int.y)
		{
			y += diffY;
			if (d >= 0)
			{
				x += diffX;
				d -= 2 * dy;
			}
			d += 2 * dx;
			image[y][x] = Palette::White;
		}
	}
}

bool IsCorrectRegionVertex(const int width, const int height, const Vec4& v)
{
	if (v.x >= width || v.x < 0) { return false; }
	if (v.y >= height || v.y < 0) { return false; }
	if (IsNaN(v.x) || IsNaN(v.y)) { return false; }
	return true;
}

void HiddenSurfaceRemovalNormalSmaple()
{
	Array<Vec4> vertices;
	Array<Vector3D<int>> indices;
	LoadObj(U"example/obj/bunny_low.obj", vertices, indices);

	// ワールド座標変換行列
	Mat4x4 worldMatrix = Mat4x4::Scale(0.9).rotatedY(90);

	// ビュー変換行列
	Vec3 origin = { 0.2,0.1,-0.05 };
	Vec3 up = { 0.0, 1.0, 0.0 };
	Vec3 lookPoint = { 0.0,0.1,0.0 };
	Vec3 z = (origin - lookPoint).normalize();
	Vec3 x = up.cross(z).normalize();
	Vec3 y = z.cross(x).normalize();

	Mat4x4 tempMat1 = Mat4x4::Set
	(
		x.x, x.y, x.z, 0,
		y.x, y.y, y.z, 0,
		z.x, z.y, z.z, 0,
		0, 0, 0, 1
	);

	Mat4x4 tempMat2 = Mat4x4::Set
	(
		1, 0, 0, -origin.x,
		0, 1, 0, -origin.y,
		0, 0, 1, -origin.z,
		0, 0, 0, 1
	);

	Mat4x4 viewMat = tempMat1 * tempMat2;

	// 透視投影行列
	double near = -1.0, fov = Math::Pi / 6.0;
	double aspect = Scene::Size().x / static_cast<double>(Scene::Size().y);
	double far = 10.0;
	double f = Cos(fov) / Sin(fov);
	Mat4x4 perspectiveMat = Mat4x4::Set
	(
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, -(far + near) / (far - near), -(2.0 * near * far) / (far - near),
		0, 0, -1.0, 0
	);

	double ff = 1.0, nn = 0.0;
	Mat4x4 viewportMat = Mat4x4::Set
	(
		Scene::Size().x / 2.0, 0, 0, Scene::Size().x / 2.0 + 0.0,
		0, -Scene::Size().y / 2.0, 0.0, Scene::Size().y / 2.0 + 0.0,
		0, 0, (ff - nn) / 2.0, (ff + nn) / 2.0,
		0, 0, 0, 1
	);

	float wx = Scene::Size().x / 2.0f, ox = 0.0f;
	float wy = Scene::Size().y / 2.0f, oy = 0.0f;

	for (auto& v : vertices)
	{
		auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(v.x, v.y, v.z, v.w), worldMatrix);
		tempV = DirectX::XMVector4Transform(tempV, viewMat.transposed());
		tempV = DirectX::XMVector4Transform(tempV, perspectiveMat.transposed());
		// 正規化デバイス座標へ
		tempV.m128_f32[0] = tempV.m128_f32[0] / tempV.m128_f32[3];
		tempV.m128_f32[1] = tempV.m128_f32[1] / tempV.m128_f32[3];
		tempV.m128_f32[2] = tempV.m128_f32[2] / tempV.m128_f32[3];
		tempV.m128_f32[3] = 1.0f;

		tempV = DirectX::XMVector4Transform(tempV, viewportMat.transposed());

		v = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
	}

	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

	int count = 0;
	for (const Vector3D<int>& polygon : indices)
	{
		const Vec4& v1 = vertices[polygon.elem(0)];
		const Vec4& v2 = vertices[polygon.elem(1)];
		const Vec4& v3 = vertices[polygon.elem(2)];

		Vec3 meshNormal =
			Cross((v2 - v1).normalize().xyz(), (v3 - v1).normalize().xyz());
		Vec3 cameraRay = (lookPoint - origin).normalize();
		if (Dot(meshNormal, cameraRay) > 0.0)
		{
			continue;
		}

		if (
			IsCorrectRegionVertex(image.width(), image.height(), v1) == false ||
			IsCorrectRegionVertex(image.width(), image.height(), v2) == false ||
			IsCorrectRegionVertex(image.width(), image.height(), v3) == false
			)
		{
			continue;
		}

		BresenhamAll(v1.xy(), v2.xy(), image);
		BresenhamAll(v2.xy(), v3.xy(), image);
		BresenhamAll(v3.xy(), v1.xy(), image);
		count++;
	}

	texture.fill(image);

	while (System::Update())
	{
		texture.draw();

		ClearPrint();

		Print << U"All: " << indices.size();
		Print << U"Draw: " << count;
	}
}
