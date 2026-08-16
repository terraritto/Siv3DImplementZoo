#pragma once
#include <Siv3D.hpp>

void MonteCarloES()
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

		bool operator==(const Trajectory& value) const
		{
			bool result = m_isPlayerUseAceEleven == value.m_isPlayerUseAceEleven;
			result &= m_playerSum == value.m_playerSum;
			result &= m_dealerCardFirst == value.m_dealerCardFirst;
			result &= m_action == value.m_action;
			return result;
		}
	};

	// 今回考慮するPolicy
	auto BehaviorPolicy = [&](const Array<double>& stateActionValues, const Array<double> stateActionCounts, Trajectory trajectory)
		{
			const int playerUseAceEleven = static_cast<int>(trajectory.m_isPlayerUseAceEleven);
			const int playerSum = trajectory.m_playerSum - 12;
			const int dealerCard = trajectory.m_dealerCardFirst - 1;
			Array<std::tuple<double, Action>> actionValues, results;
			for (auto action : actions)
			{
				int index;
				const int actionIndex = static_cast<int>(action);
				switch (playerUseAceEleven)
				{
				case 0:
					if (actionIndex == 0) { index = 0; }
					else { index = 2; }
					break;

				case 1:
					if (actionIndex == 0) { index = 1; }
					else { index = 3; }
				}

				double value = stateActionValues[(playerSum * 10 + dealerCard) + 100 * index];
				double count = stateActionCounts[(playerSum * 10 + dealerCard) + 100 * index];
				actionValues.push_back({ value / count, action });
			}

			// 最大値を放り込んでおく
			const auto maxData = *std::max_element(actionValues.begin(), actionValues.end(), [](auto lhs, auto rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
			for (auto value : actionValues)
			{
				if (std::get<0>(maxData) == std::get<0>(value))
				{
					results.push_back(value);
				}
			}

			// 一番大きい手の中からランダムに返す
			return std::get<1>(results.choice());
		};

	auto Play = [&](Array<Trajectory>& trajectories, int& reward, const bool isFirstEpisode, const Trajectory& initialState, const Array<double>& stateActionValues, const Array<double> stateActionCounts)
		{
			// プレイヤー側
			int playerSum = 0; // プレイヤーの合計点
			bool isPlayerUseAceEleven = false; // Aceを11として使う？

			// ディーラー側
			int dealerSum = 0; // ディーラーの得点
			int dealerCardFirst = 0; // 1枚目
			int dealerCardSecond = 0; // 2枚目
			bool isDealerUseAceEleven = false; // Aceを11として使う？

			// 初期状態から値を設定
			isPlayerUseAceEleven = initialState.m_isPlayerUseAceEleven;
			playerSum = initialState.m_playerSum;
			dealerCardFirst = initialState.m_dealerCardFirst;

			// ディーラーも最初の2枚を引いて、得点調整
			dealerCardSecond = GetCard();
			dealerSum = CardValue(dealerCardFirst) + CardValue(dealerCardSecond);
			isDealerUseAceEleven = Array<int>{ dealerCardFirst, dealerCardSecond }.contains(1); // 1があればAce確定
			if (dealerSum >= 22)
			{
				// 2枚ともAceの場合のみ22になるので、1の方を採用する
				dealerSum -= 10;
			}

			// 条件は整ったので、実際にゲーム開始！
			// プレイヤーのターン
			bool isInitial = true;
			while (true)
			{
				// 行動を決定
				Action action;
				if (isInitial)
				{
					// 初回はすでに決めていた行動にする
					action = initialState.m_action;
					isInitial = false;
				}
				else
				{
					// 初回以外はポリシーに従う
					if (isFirstEpisode)
					{
						// 最初の処理の場合はそもそも初期値なので、適当に選ぶようにする
						action = PolicyPlayer(playerSum);
					}
					else
					{
						Trajectory policyTrajectory;
						policyTrajectory.m_playerSum = playerSum;
						policyTrajectory.m_isPlayerUseAceEleven = isPlayerUseAceEleven;
						policyTrajectory.m_dealerCardFirst = dealerCardFirst;

						// 2回目以降はGreedyに選択
						action = BehaviorPolicy(stateActionValues, stateActionCounts, policyTrajectory);
					}
				}

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

	auto MonteCarloES = [&](int episodes)
		{
			// aceを11として使うかの判定による状態 x2
			// hitかstandかの状態 x2
			// 全プレイヤーとディーラーの状態が100なので、合計は100x2x2=400の状態となる
			constexpr int ALL_STATE = 100 * 2 * 2;
			Array<double> stateActionValues(ALL_STATE, 0.0);
			Array<double> stateActionCounts(ALL_STATE, 1.0);
			for (int i = 0; i < episodes; i++)
			{
				// 初期状態を生成
				Trajectory initState;
				initState.m_isPlayerUseAceEleven = RandomBool();
				initState.m_playerSum = Random(12, 21);
				initState.m_dealerCardFirst = Random(1, 10);
				initState.m_action = actions.choice();

				// ブラックジャックを1回プレイ
				int reward = 0; Array<Trajectory> trajectories;
				Play(trajectories, reward, i == 0, initState, stateActionValues, stateActionCounts);

				Array<Trajectory> firstVisitChecks;
				for (const auto& trajectory : trajectories)
				{
					// 同じものは評価しないように
					if (firstVisitChecks.contains(trajectory))
					{
						continue;
					}
					firstVisitChecks.push_back(trajectory); // 登録

					// 値を正規化しておく
					const int playerUseAceEleven = static_cast<int>(trajectory.m_isPlayerUseAceEleven);
					const int actionIndex = static_cast<int>(trajectory.m_action);
					int playerSum = trajectory.m_playerSum - 12;
					int dealerCard = trajectory.m_dealerCardFirst - 1;

					// indexを確定
					int index;
					switch (playerUseAceEleven)
					{
					case 0:
						if (actionIndex == 0) { index = 0; }
						else { index = 2; }
						break;

					case 1:
						if (actionIndex == 0) { index = 1; }
						else { index = 3; }
					}

					stateActionValues[playerSum * 10 + dealerCard + 100 * index] += reward; // 報酬を加算
					stateActionCounts[playerSum * 10 + dealerCard + 100 * index] += 1.0; // Aceを11として利用した回数を加算
				}
			}

			// 平均にしておく
			for (int i = 0; i < stateActionValues.size(); ++i)
			{
				stateActionValues[i] /= stateActionCounts[i];
			}

			return stateActionValues;
		};

	const auto stateActionValues = MonteCarloES(500000);

	Scene::SetBackground(Palette::White);

	enum class ShowType
	{
		ValueNoUsableAce, ValueUsableAce, PolicyNoUsableAce, PolicyUsableAce,
	};

	Array<double> valueNoUsableAce, valueUsableAce;
	Array<double> policyNoUsableAce, policyUsableAce;
	for (int i = 0; i < 100; i++)
	{
		double actionFirst = stateActionValues[i + 100 * 0];
		double actionSecond = stateActionValues[i + 100 * 2];
		valueNoUsableAce.push_back(Max(actionFirst, actionSecond));
		policyNoUsableAce.push_back(actionFirst < actionSecond);

		actionFirst = stateActionValues[i + 100 * 1];
		actionSecond = stateActionValues[i + 100 * 3];
		valueUsableAce.push_back(Max(actionFirst, actionSecond));
		policyUsableAce.push_back(actionFirst < actionSecond);
	}

	Array<Array<double>> results{
		valueNoUsableAce, valueUsableAce,
		policyNoUsableAce, policyUsableAce
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

		if (SimpleGUI::Button(U"Value", { 150, 50 }, 80)) { index = 0; }
		if (SimpleGUI::Button(U"Value A", { 250, 50 }, 80)) { index = 1; }
		if (SimpleGUI::Button(U"Policy", { 350, 50 }, 80)) { index = 2; }
		if (SimpleGUI::Button(U"Policy A", { 450, 50 }, 80)) { index = 3; }
	}
}
