#pragma once
#include <Siv3D.hpp>

void BasisSample()
{
	Vec3 x{ 1,0,0 }; Vec3 y{ 0,1,0 }; Vec3 z{ 0,0,1 };
	Vec3 u{ 1.0 / Sqrt(2.0), -1.0 / Sqrt(2.0), 0 };
	Vec3 v = Cross(z, u).normalized(); Vec3 w = Cross(u, v);

	Vec3 p = { 0.2,0.3,0 };

	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ -1, 0, -1 } };

	Print << U"x:" << x;
	Print << U"y:" << y;
	Print << U"z:" << z;
	Print << U"u:" << u;
	Print << U"v:" << v;
	Print << U"w:" << w;


	bool isXYZ = true;
	while (System::Update())
	{
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			if (isXYZ)
			{
				Line3D{ Vec3::Zero(), x }.draw(Palette::Red);
				Line3D{ Vec3::Zero(), y }.draw(Palette::Blue);
				Line3D{ Vec3::Zero(), z }.draw(Palette::Green);

				Sphere{ Dot(p, x) / x.length() * x, 0.01 }.draw(Palette::Red);
				Sphere{ Dot(p, y) / v.length() * y, 0.01 }.draw(Palette::Blue);
				Sphere{ Dot(p, z) / v.length() * z, 0.01 }.draw(Palette::Green);
			}
			else
			{
				Line3D{ Vec3::Zero(), u }.draw(Palette::Cyan);
				Line3D{ Vec3::Zero(), -u }.draw(Palette::Cyan);
				Line3D{ Vec3::Zero(), v }.draw(Palette::Magenta);
				Line3D{ Vec3::Zero(), -v }.draw(Palette::Magenta);
				Line3D{ Vec3::Zero(), w }.draw(Palette::Yellow);
				Line3D{ Vec3::Zero(), -w }.draw(Palette::Yellow);

				Sphere{ Dot(p, u) / u.length() * u, 0.01 }.draw(Palette::Cyan);
				Sphere{ Dot(p, v) / v.length() * v, 0.01 }.draw(Palette::Magenta);
				Sphere{ Dot(p, w) / w.length() * w, 0.01 }.draw(Palette::Yellow);

				Vec3 pp{ Dot(p,u), Dot(p,v), Dot(p,w) };
				Sphere{ pp, 0.01 }.draw(Palette::Black);

				Sphere{ Dot(pp, u) / u.length() * u, 0.01 }.draw(Palette::Cyan);
				Sphere{ Dot(pp, v) / v.length() * v, 0.01 }.draw(Palette::Magenta);
				Sphere{ Dot(pp, w) / w.length() * w, 0.01 }.draw(Palette::Yellow);
			}

			// point
			Sphere{ p, 0.01 }.draw(Palette::White);
		}

		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}

		if (KeySpace.down()) { isXYZ = !isXYZ; }
	}
}
