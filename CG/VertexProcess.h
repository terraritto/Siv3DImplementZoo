#pragma once

#pragma once
#include "Siv3D.hpp"

void LoadObj(String filePath, Array<Vec4>& vertices)
{
	TextReader reader{ filePath };

	if (reader)
	{
		String line;
		while (reader.readLine(line))
		{
			if (not line) { continue; }
			const Array<String> items = line.split(U' ');

			if (items[0] != U"v")
			{
				continue;
			}

			vertices.push_back(Vec4{ Parse<double>(items[2]), Parse<double>(items[3]), Parse<double>(items[4]), 1.0 });
		}
	}
}

void VertexProcessSmaple()
{
	Array<Vec4> vertices;
	LoadObj(U"example/obj/bunny.obj", vertices);

	// ワールド座標変換行列
	Mat4x4 worldMatrix = Mat4x4::Scale(0.9);

	// ビュー変換行列
	Vec3 origin = { 0.2,0.8,-1.5 };
	Vec3 up = { 0.0, 1.0, 0.0 };
	Vec3 lookPoint = { 0.0,0.8,0.0 };
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
	double f = Cos(fov)/Sin(fov);
	Mat4x4 perspectiveMat = Mat4x4::Set
	(
		f/aspect, 0, 0, 0,
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
		auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(v.x,v.y,v.z,v.w), worldMatrix);
		tempV = DirectX::XMVector4Transform(tempV, viewMat.transposed());
		tempV = DirectX::XMVector4Transform(tempV, perspectiveMat.transposed());
		// 正規化デバイス座標へ
		tempV.m128_f32[0] = tempV.m128_f32[0] / tempV.m128_f32[3];
		tempV.m128_f32[1] = tempV.m128_f32[1] / tempV.m128_f32[3];
		tempV.m128_f32[2] = tempV.m128_f32[2] / tempV.m128_f32[3];
		tempV.m128_f32[3] = 1.0f;

		if (tempV.m128_f32[0] < -1.0 || tempV.m128_f32[0] > 1.0) { continue; }
		if (tempV.m128_f32[1] < -1.0 || tempV.m128_f32[1] > 1.0) { continue; }

		tempV.m128_f32[0] = tempV.m128_f32[0] * wx + wx + ox;
		tempV.m128_f32[1] = tempV.m128_f32[1] * -wy + wy + oy;

		v = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
	}

	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

	int count = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		auto& v = vertices[i];

		if (v.x >= image.width() || v.x < 0) { continue; }
		if (v.y >= image.height() || v.y < 0) { continue; }
		if (IsNaN(v.x) || IsNaN(v.y)) { continue; }
		image[static_cast<int>(v.y)][static_cast<int>(v.x)] = Palette::White;
	}

	texture.fill(image);

	while (System::Update())
	{
		texture.draw();
	}
}
