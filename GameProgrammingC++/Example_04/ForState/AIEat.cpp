#include "AIEat.h"
#include "../AIComponent.h"
#include "../Game.h"
#include "Food.h"
#include "Eater.h"

void AIEat::Update()
{
	Print << GetName();

	if (m_timer.reachedZero())
	{
		m_timer.reset();

		// クールタイムが終わったら動く
		auto owner = m_owner.lock();
		owner->ChangeState(U"find");
	}
}

void AIEat::OnEnter()
{
	auto owner = m_owner.lock();
	auto actor = std::dynamic_pointer_cast<Eater>(owner->GetOwner().lock());
	auto game = owner->GetGame().lock();

	// 一番近いFoodを探す
	auto foods = game->GetActorsFromClass<Food>();

	// 一番近いのを探して削除
	float minDistance = std::numeric_limits<float>::max();
	std::shared_ptr<Food> targetFood;
	for (auto food : foods)
	{
		auto pFood = food.lock();

		auto distance = (pFood->GetPosition() - actor->GetPosition()).length();
		if (distance < minDistance)
		{
			minDistance = distance;
			targetFood = pFood;
		}
	}

	targetFood->SetState(Actor::State::Dead);

	// タイマーをスタート
	m_timer.restart();
}

void AIEat::OnExit()
{
}
