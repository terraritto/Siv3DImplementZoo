#include "AIFind.h"
#include "../AIComponent.h"
#include "../Game.h"
#include "Food.h"
#include "Eater.h"

void AIFind::Update()
{
	Print << GetName();

	if (m_timer.reachedZero())
	{
		m_timer.reset();

		// クールタイムが終わったら動く
		auto owner = m_owner.lock();
		owner->ChangeState(U"move");
	}
}

void AIFind::OnEnter()
{
	auto owner = m_owner.lock();
	auto actor = std::dynamic_pointer_cast<Eater>(owner->GetOwner().lock());
	auto game = owner->GetGame().lock();

	// 一番近いFoodを探す
	auto foods = game->GetActorsFromClass<Food>();

	// 全部食べてたら終了
	if (foods.size() == 0)
	{
		owner->ChangeState(U"end");
		return;
	}

	// 一番近いのを探して設定
	float minDistance = std::numeric_limits<float>::max();
	Float2 target{};
	for (auto food : foods)
	{
		auto pFood = food.lock();

		auto distance = (pFood->GetPosition() - actor->GetPosition()).length();
		if (distance < minDistance)
		{
			minDistance = distance;
			target = pFood->GetPosition();
		}
	}

	actor->SetTargetPosition(target);

	// タイマーをスタート
	m_timer.restart();
}

void AIFind::OnExit()
{
}
