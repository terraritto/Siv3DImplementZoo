#pragma once
#include <Siv3D.hpp>

void MagicalTrialFadeSample()
{
	struct ShaderParam
	{
		float m_mod = 0.1f;
		float m_scale = 0.0f;
		float m_gapScale = 0.1f;
		bool m_isFadeIn = false;
	};

	constexpr ColorF backgroundColor{ 0.3, 0.4, 0.5 };
	const MSRenderTexture renderTexture{ Scene::Size() };

	const PixelShader pixelShader = HLSL{ U"example/shader/hlsl/magical_trial_fade.hlsl", U"PS" };
	ConstantBuffer<ShaderParam> cb;

	enum class Fade { In, Out };
	Fade fadeState = Fade::Out;
	bool isAnim = false;
	Timer timer{ 1.0s };

	constexpr auto BALL_RADIUS = 5;
	constexpr auto MAX_BALL = 100;
	constexpr auto BALL_SPEED = 100;

	struct Ball
	{
		Ball(Vec2 position, Vec2 speed)
			: m_circle(Circle{ position, BALL_RADIUS })
			, m_speed(speed)
			, m_isLife(true)
		{}

		Circle m_circle;
		Vec2 m_speed;
		bool m_isLife;
	};

	Array<Ball> balls;
	auto region = Scene::Size();

	auto GetRandomSpeed = [](bool isAbs = false) {return Vec2{
		Random(isAbs ? 0 : -BALL_SPEED, BALL_SPEED),
		Random(isAbs ? 0 : -BALL_SPEED, BALL_SPEED) }; };
	auto GetRandomPos = [&]() {return Vec2{ Random(0, region.x), Random(0, region.y) }; };

	auto CreateBall = [&]()
		{
			if (balls.size() >= MAX_BALL) { return; }

			balls.push_back(Ball{ GetRandomPos(), GetRandomSpeed() });
		};

	auto BallMove = [&]()
		{
			for (auto& ball : balls)
			{
				ball.m_circle.moveBy(ball.m_speed * Scene::DeltaTime());

				if (ball.m_circle.x < -BALL_RADIUS) { ball.m_speed.x *= -1; }
				if (ball.m_circle.y < -BALL_RADIUS) { ball.m_speed.y *= -1; }
				if (region.x+BALL_RADIUS < ball.m_circle.x) { ball.m_speed.x *= -1; }
				if (region.y+BALL_RADIUS < ball.m_circle.y) { ball.m_speed.y *= -1; }
			}
		};

	auto BallCollision = [&]()
		{
			for (int i = 0; i < balls.size() - 1; i++)
			{
				for (int j = i + 1; j < balls.size(); j++)
				{
					auto& ballA = balls[i]; auto& ballB = balls[j];
					if (ballA.m_circle.intersects(ballB.m_circle))
					{
						if (RandomBool()) { CreateBall(); }

						if (RandomBool(0.1))
						{
							ballA.m_isLife = false; ballB.m_isLife = false;
							continue;
						}

						ballA.m_speed = (ballA.m_circle.center - ballB.m_circle.center).normalized() * GetRandomSpeed(true);
					}
				}
			}
		};

	auto BallLife = [&]()
		{
			balls.remove_if([](const Ball& ball) { return !ball.m_isLife; });
		};

	// 生成しとく
	for (auto i : step(30)) { CreateBall(); }

	while (System::Update())
	{
		BallMove();
		BallCollision();
		BallLife();

		renderTexture.clear(backgroundColor);

		if (isAnim)
		{
			if (timer.reachedZero())
			{
				isAnim = false;
				switch (fadeState)
				{
				case Fade::In:
					fadeState = Fade::Out;
					cb->m_isFadeIn = false;
					break;

				case Fade::Out:
					fadeState = Fade::In;
					cb->m_isFadeIn = true;
					break;
				}

				cb->m_scale = 0.0f;
			}
			else
			{
				cb->m_scale = timer.progress0_1() * 0.4;
			}
		}

		// ballの描画
		{
			const ScopedRenderTarget2D target{ renderTexture };

			for (const auto& ball : balls)
			{
				ball.m_circle.draw(Palette::Red);
			}
		}

		Graphics2D::Flush();
		renderTexture.resolve();

		// Pixel Shader
		{
			Graphics2D::SetPSConstantBuffer(1, cb);
			const ScopedCustomShader2D shader{ pixelShader };
			renderTexture.draw();
		}

		double v = cb->m_mod;
		SimpleGUI::Slider(U"mod", v, 0.01, 1.0, Vec2{ 10, 10 }); cb->m_mod = static_cast<float>(v);
		v = cb->m_gapScale;
		SimpleGUI::Slider(U"gap", v, 0.001, 0.1, Vec2{ 10, 90 }); cb->m_gapScale = static_cast<float>(v);

		if (SimpleGUI::Button(U"Fade", Vec2{10,140}))
		{
			if (!timer.isRunning())
			{
				timer.restart();
				isAnim = true;
			}
		}
	}
}
