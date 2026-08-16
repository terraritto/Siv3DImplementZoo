#pragma once
#include <Siv3D.hpp>

void MonteCarloOnPolicy()
{
	// ブラックジャックにおける行動
	enum class Action
	{
		Hit = 0,	// ヒット
		Stand = 1	// スタンド
	};
	const Array<Action> actions = { Action::Hit, Action::Stand };

	// Playerの行動ポリシー
	Array<Action> playerPolicy; playerPolicy.resize(22, Action::Hit);
	// [12,19]の場合はヒット,[20,21]はスタンド
	for (int i = 12; i < 20; i++) { playerPolicy[i] = Action::Hit; }
	playerPolicy[20] = Action::Stand; playerPolicy[21] = Action::Stand;
	auto PolicyPlayer = [&](int playerSum) { return playerPolicy[playerSum]; };

	// Dealerの行動ポリシー
	Array<Action> dealerPolicy; dealerPolicy.resize(22, Action::Hit);
	// [12,16]の場合はヒット,[17,21]はスタンド
	for (int i = 12; i < 17; i++) { dealerPolicy[i] = Action::Hit; }
	for (int i = 17; i < 22; i++) { dealerPolicy[i] = Action::Stand; }
	auto PolicyDealer = [&](int dealerSum) {return dealerPolicy[dealerSum]; };

	// カードの取得
	auto GetCard = []()
		{
			int card = Random<int>(1, 13); // [1,2,...,J,Q,K]
			return Min(card, 10); // J,Q,Kは10と同じ
		};

	// Aceを11とするように値を加工
	auto CardValue = [](int value) { return value == 1 ? 11 : value; };

	struct Trajectory
	{
		bool m_isPlayerUseAceEleven;
		int m_playerSum;
		int m_dealerCardFirst;
		Action m_action;
	};

	auto Play = [&](Array<Trajectory>& trajectories, int& reward)
		{
			// プレイヤー側
			int playerSum = 0; // プレイヤーの合計点
			bool isPlayerUseAceEleven = false; // Aceを11として使う？

			// ディーラー側
			int dealerSum = 0; // ディーラーの得点
			int dealerCardFirst = 0; // 1枚目
			int dealerCardSecond = 0; // 2枚目
			bool isDealerUseAceEleven = false; // Aceを11として使う？

			// 行動ポリシー範囲内までカードを引く
			while (playerSum < 12)
			{
				int card = GetCard();
				playerSum += CardValue(card);

				if (playerSum > 21)
				{
					// 手持ちが11の時にAceを引いた場合,22になる可能性あり
					// この場合は1として換算するために10を引いておく
					playerSum -= 10;
				}
				else
				{
					// Aceを引いてたら持ってるものに換算
					isPlayerUseAceEleven |= (card == 1);
				}
			}

			// ディーラーも最初の2枚を引いて、得点調整
			dealerCardFirst = GetCard(); dealerCardSecond = GetCard();
			dealerSum = CardValue(dealerCardFirst) + CardValue(dealerCardSecond);
			isDealerUseAceEleven = Array<int>{ dealerCardFirst, dealerCardSecond }.contains(1); // 1があればAce確定
			if (dealerSum >= 22)
			{
				// 2枚ともAceの場合のみ22になるので、1の方を採用する
				dealerSum -= 10;
			}

			// 条件は整ったので、実際にゲーム開始！
			// プレイヤーのターン
			while (true)
			{
				// 行動を決定
				Action action = PolicyPlayer(playerSum);

				// 状態を保存
				Trajectory trajectory;
				trajectory.m_isPlayerUseAceEleven = isPlayerUseAceEleven;
				trajectory.m_playerSum = playerSum;
				trajectory.m_dealerCardFirst = dealerCardFirst;
				trajectory.m_action = action;
				trajectories.push_back(trajectory);

				// スタンドしてるなら手が決定
				if (action == Action::Stand) { break; }

				// ヒット、カードを追加
				int card = GetCard();
				playerSum += CardValue(card);

				// エース周りの特殊処理
				int aceCount = static_cast<int>(isPlayerUseAceEleven);
				aceCount += card == 1 ? 1 : 0; // 引いたカードがAceなら+1しておく

				while (playerSum > 21 && aceCount > 0)
				{
					// バーストしてる + Aceを持ってる場合,Aceを11から1に変える
					playerSum -= 10;
					aceCount--;
				}

				if (playerSum > 21)
				{
					// バーストしてるなら終了
					reward = -1;
					return;
				}

				// Aceを11で持ってるか(2枚はない、足したら22になるので)
				isPlayerUseAceEleven = (aceCount == 1);
			}

			// ディーラーのターン
			while (true)
			{
				Action action = PolicyDealer(dealerSum);

				// スタンドしてるなら手が決定
				if (action == Action::Stand) { break; }

				// ヒット、カードを追加
				int card = GetCard();
				dealerSum += CardValue(card);

				// エース周りの特殊処理
				int aceCount = static_cast<int>(isDealerUseAceEleven);
				aceCount += card == 1 ? 1 : 0; // 引いたカードがAceなら+1しておく

				while (dealerSum > 21 && aceCount > 0)
				{
					// バーストしてる + Aceを持ってる場合,Aceを11から1に変える
					dealerSum -= 10;
					aceCount--;
				}

				if (dealerSum > 21)
				{
					// バーストしてるなら終了
					reward = +1;
					return;
				}

				// Aceを11で持ってるか(2枚はない、足したら22になるので)
				isDealerUseAceEleven = (aceCount == 1);
			}

			// 勝敗
			if (playerSum > dealerSum) { reward = 1; return; } // 勝ち
			if (playerSum == dealerSum) { reward = 0; return; } // 引き分け
			reward = -1; return; // 負け
		};

	auto MonteCarloOnPolicy = [&](int episodes)
	{
		Array<double> aceHaveStates(100, 0.0), aceHaveCount(100, 1.0);
		Array<double> aceNoHaveStates(100, 0.0), aceNoHaveCount(100, 1.0);
		for (int i = 0; i < episodes; i++)
		{
			// ブラックジャックを1回プレイ
			int reward = 0; Array<Trajectory> trajectories;
			Play(trajectories, reward);

			for (const auto& trajectory : trajectories)
			{
				int playerSum = trajectory.m_playerSum;
				int dealerCard = trajectory.m_dealerCardFirst;

				// [12,21] -> [0,9]に変換
				playerSum -= 12;
				// [1, 10] -> [0,9]に変換
				dealerCard--;

				// Aceによって配列を取得
				auto& states = trajectory.m_isPlayerUseAceEleven ? aceHaveStates : aceNoHaveStates;
				auto& counts = trajectory.m_isPlayerUseAceEleven ? aceHaveCount : aceNoHaveCount;

				states[playerSum * 10 + dealerCard] += reward; // 報酬を加算
				counts[playerSum * 10 + dealerCard] += 1.0; // Aceを11として利用した回数を加算
			}
		}

		// 平均にしておく
		for (int i = 0; i < aceHaveCount.size(); ++i)
		{
			aceHaveStates[i] /= aceHaveCount[i];
			aceNoHaveStates[i] /= aceNoHaveCount[i];
		}

		return std::make_pair(aceHaveStates, aceNoHaveStates);
	};

	auto trial10k = MonteCarloOnPolicy(10000);
	auto trial500k = MonteCarloOnPolicy(500000);

	Scene::SetBackground(Palette::White);

	enum class ShowType
	{
		Trial10k, Triak10kNone, Trial500k, Trial500kNone,
	};

	Array<Array<double>> results{
		trial10k.first, trial10k.second,
		trial500k.first, trial500k.second
	};
	int index = 0;

	constexpr Vec2 CENTER_START = { 150,100 };
	constexpr double GRID_SIZE = 25;
	constexpr double GRID_OFFSET = 2;
	constexpr double GRID_OFFSET_SIZE = GRID_SIZE + GRID_OFFSET;

	while (System::Update())
	{
		auto value = results[index];

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				const double v = Clamp(value[i * 10 + j], -1.0, 1.0) * 0.5 + 0.5;
				auto color = Colormap01(v, s3d::ColormapType::Heat);

				Vec2 center = CENTER_START + Vec2{ j * GRID_OFFSET_SIZE, (10 - i) * GRID_OFFSET_SIZE };
				RectF{ Arg::center(center), GRID_SIZE, GRID_SIZE }.draw(color);
			}
		}

		if (SimpleGUI::Button(U"10k", { 150, 50 }, 80)) { index = 0; }
		if (SimpleGUI::Button(U"10kNone", { 250, 50 }, 80)) { index = 1; }
		if (SimpleGUI::Button(U"500k", { 350, 50 }, 80)) { index = 2; }
		if (SimpleGUI::Button(U"500kNone", { 450, 50 }, 80)) { index = 3; }
	}
}
