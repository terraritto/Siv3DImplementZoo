# include <Siv3D.hpp>

void SensorTest()
{
	// ウィンドウを 1280x720 にリサイズ
	Window::Resize(1280, 720);

	// 物理シミュレーションのステップ時間と重力
	constexpr double StepTime = (1.0 / 200.0);
	double accumulatedTime = 0.0;
	constexpr double Gravity = 980;

	// 2D 物理演算のワールド
	P2World world{ Gravity };

	// 地面と壁
	const P2Body ground = world.createRect(P2Static, Vec2{ 0, 0 }, RectF{ Vec2{-600, 0}, SizeF{1200, 80} });
	const P2Body wallLeft = world.createLine(P2Static, Vec2{ -600, 0 }, Line{ 0, 0, 0, -400 });
	const P2Body wallRight = world.createLine(P2Static, Vec2{ 600, 0 }, Line{ 0, 0, 0, -400 });

	// プレイヤーを表す物体
	constexpr Vec2 respawnPos{ -400, -100 };
	P2Body player = world.createRect(P2Dynamic, respawnPos, SizeF{ 50, 50 })
		.setFixedRotation(true);
	// 足元にセンサーを追加
	// player.addRectSensor(RectF{ Vec2{-10, 40}, SizeF{20, 20} });
	P2Body playerFootSensor = world.createRectSensor(P2Dynamic, respawnPos, RectF{ Vec2{-20, 15}, SizeF{40, 20} })
		.setFixedRotation(true)
		.setGravityScale(0.0);
	P2DistanceJoint playerFootJoint = world.createDistanceJoint(player, respawnPos, playerFootSensor, respawnPos, 0);
	bool playerOnGround = false;

	// 2D カメラ
	Camera2D camera{ Vec2{ 0, -300 }, 1.0, CameraControl::Mouse };

	// 各形状
	const Circle circle = Circle{ Vec2::Zero(), 30 };
	const Triangle triangle = Triangle{ Vec2::Zero().movedBy(0, 8), 60 };
	const RectF rect = RectF{ Vec2{-30, -30}, SizeF{60, 60} };
	const Quad quad = RectF{ Vec2{-20, -30}, SizeF{40, 60} }.shearedX(20);
	const Polygon polygon = Shape2D::NStar(8, 30, 10);
	const Line line = Line{ Vec2{0, -30}, Vec2{0, 30} };
	const LineString lineString = LineString{ Vec2{-30, -30}, Vec2{30, -10}, Vec2{-30, 10}, Vec2{30, 30} };

	// 各センサー
	const P2Body sensorCircle = world.createCircleSensor(P2Static, Vec2{ -480, -60 }, circle);
	const P2Body sensorTriangle = world.createTriangleSensor(P2Static, Vec2{ -320, -60 }, triangle);
	const P2Body sensorRect = world.createRectSensor(P2Static, Vec2{ -160, -60 }, rect);
	const P2Body sensorQuad = world.createQuadSensor(P2Static, Vec2{ 0, -60 }, quad);
	const P2Body sensorPolygon = world.createPolygonSensor(P2Static, Vec2{ 160, -60 }, polygon);
	const P2Body sensorLine = world.createLineSensor(P2Static, Vec2{ 320, -60 }, line);
	const P2Body sensorLineString = world.createLineStringSensor(P2Static, Vec2{ 480, -60 }, lineString);

	// 各壁
	const P2Body wallCircle = world.createCircle(P2Static, Vec2{ -480, -180 }, circle);
	const P2Body wallTriangle = world.createTriangle(P2Static, Vec2{ -320, -180 }, triangle);
	const P2Body wallRect = world.createRect(P2Static, Vec2{ -160, -180 }, rect);
	const P2Body wallQuad = world.createQuad(P2Static, Vec2{ 0, -180 }, quad);
	const P2Body wallPolygon = world.createPolygon(P2Static, Vec2{ 160, -180 }, polygon);
	const P2Body wallLine = world.createLine(P2Static, Vec2{ 320, -180 }, line);
	const P2Body wallLineString = world.createLineString(P2Static, Vec2{ 480, -180 }, lineString);

	// センサーの色
	constexpr Color sensorColor = { Palette::Green, 127 };
	constexpr Color sensorFrameColor = Palette::Green;
	constexpr Color sensorCollidingColor = { Palette::Red, 127 };
	constexpr Color sensorCollidingFrameColor = Palette::Red;
	// 壁の色
	constexpr Color wallColor = { Palette::Gray };

	// センサー用のボディ一覧
	HashSet<P2BodyID> sensorSet;
	sensorSet.emplace(playerFootSensor.id());
	sensorSet.emplace(sensorCircle.id());
	sensorSet.emplace(sensorTriangle.id());
	sensorSet.emplace(sensorRect.id());
	sensorSet.emplace(sensorQuad.id());
	sensorSet.emplace(sensorPolygon.id());
	sensorSet.emplace(sensorLine.id());
	sensorSet.emplace(sensorLineString.id());

	// 壁用のボディ一覧
	HashSet<P2BodyID> wallSet;
	wallSet.emplace(ground.id());
	wallSet.emplace(wallRight.id());
	wallSet.emplace(wallLeft.id());
	wallSet.emplace(wallCircle.id());
	wallSet.emplace(wallTriangle.id());
	wallSet.emplace(wallRect.id());
	wallSet.emplace(wallQuad.id());
	wallSet.emplace(wallPolygon.id());
	wallSet.emplace(wallLine.id());
	wallSet.emplace(wallLineString.id());

	// センサーの描画
	auto drawSensor = [&](const P2Body& sensorBody, const bool isColliding, double const frameThickness)
		{
			sensorBody.draw(isColliding ? sensorCollidingColor : sensorColor).drawFrame(frameThickness, isColliding ? sensorCollidingFrameColor : sensorFrameColor);
		};

	while (System::Update())
	{
		ClearPrint();
		Print << player.getVelocity();

		for (accumulatedTime += Scene::DeltaTime(); StepTime <= accumulatedTime; accumulatedTime -= StepTime)
		{
			// [←][A] キーで左に移動
			if (KeyLeft.pressed() || KeyA.pressed())
			{
				player.applyForce(Vec2{ -40000, 0 } *StepTime);
			}

			// [→][D] キーで右に移動
			if (KeyRight.pressed() || KeyD.pressed())
			{
				player.applyForce(Vec2{ 40000, 0 } *StepTime);
			}

			world.update(StepTime);
		}

		// センサーの接触の検出
		HashTable<P2BodyID, Array<P2BodyID>> sensorCollisionTable;
		for (auto&& [pair, collision] : world.getCollisions())
		{
			bool aIsSensor = sensorSet.contains(pair.a);
			bool bIsSensor = sensorSet.contains(pair.b);

			if (!(aIsSensor ^ bIsSensor))
			{
				// 両方がセンサー、または両方がセンサーでない接触はスキップ
				continue;
			}

			if (aIsSensor)
			{
				//

				// センサー（pair.a）の接触先を追加
				sensorCollisionTable[pair.a].emplace_back(pair.b);
			}
			else if (bIsSensor)
			{
				// センサー（pair.b）の接触先を追加
				sensorCollisionTable[pair.b].emplace_back(pair.a);
			}
		}

		// プレイヤーの接地の判定
		playerOnGround = false;
		for (auto it = sensorCollisionTable[playerFootSensor.id()].begin(); it != sensorCollisionTable[playerFootSensor.id()].end(); ++it)
		{
			if (wallSet.contains(*it))
			{
				playerOnGround = true;
				break;
			}
		}

		if (playerOnGround)
		{
			Print << U"地面に接しています";
		}

		// その他のセンサーのプレイヤー接触判定
		HashSet<P2BodyID> sensorIsCollidingSet;
		for (auto sensorIt = sensorSet.begin(); sensorIt != sensorSet.end(); ++sensorIt)
		{
			for (auto sensorCollisionIt = sensorCollisionTable[*sensorIt].begin(); sensorCollisionIt != sensorCollisionTable[*sensorIt].end(); ++sensorCollisionIt)
			{
				if (*sensorCollisionIt == player.id())
				{
					sensorIsCollidingSet.emplace(*sensorIt);
					break;
				}
			}
		}

		// [↑][W] キーでジャンプ
		if (playerOnGround && (KeyUp.down() || KeyW.down()))
		{
			player.applyLinearImpulse(Vec2{ 0, -200 });
		}

		// ----- 描画
		camera.update();
		{
			const auto t = camera.createTransformer();

			// 壁と地面を描画
			ground.draw(wallColor);
			wallLeft.draw(wallColor);
			wallRight.draw(wallColor);
			wallCircle.draw(wallColor);
			wallTriangle.draw(wallColor);
			wallRect.draw(wallColor);
			wallQuad.draw(wallColor);
			wallPolygon.draw(wallColor);
			wallLine.draw(wallColor);
			wallLineString.draw(wallColor);

			// プレイヤーの描画
			// player.as<P2Rect>(0)->getQuad().draw();
			// player.as<P2Rect>(1)->getQuad().draw(sensorColor).drawFrame(1.0, sensorFrameColor);
			player.draw();
			drawSensor(playerFootSensor, playerOnGround, 1.0);

			// センサーを描画
			drawSensor(sensorCircle, sensorIsCollidingSet.contains(sensorCircle.id()), 3.0);
			drawSensor(sensorTriangle, sensorIsCollidingSet.contains(sensorTriangle.id()), 3.0);
			drawSensor(sensorRect, sensorIsCollidingSet.contains(sensorRect.id()), 3.0);
			drawSensor(sensorQuad, sensorIsCollidingSet.contains(sensorQuad.id()), 3.0);
			drawSensor(sensorPolygon, sensorIsCollidingSet.contains(sensorPolygon.id()), 3.0);
			sensorLine.draw(sensorIsCollidingSet.contains(sensorLine.id()) ? sensorCollidingFrameColor : sensorFrameColor);
			sensorLineString.draw(sensorIsCollidingSet.contains(sensorLineString.id()) ? sensorCollidingFrameColor : sensorFrameColor);
		}

		camera.draw(Palette::Orange);
	}
}
