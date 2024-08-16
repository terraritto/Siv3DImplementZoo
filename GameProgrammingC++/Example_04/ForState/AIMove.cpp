#include "AIMove.h"
#include "../AIComponent.h"
#include "../Game.h"
#include "Food.h"
#include "Eater.h"


void AIMove::Update()
{
	Print << GetName();

	auto owner = m_owner.lock();
	auto actor = std::dynamic_pointer_cast<Eater>(owner->GetOwner().lock());
	auto game = owner->GetGame().lock();

	// 移動
	auto dir = (actor->GetTargetPosition() - actor->GetPosition()).normalized();
	actor->SetPosition(actor->GetPosition() + dir * m_speed * Scene::DeltaTime());

	// 食べ物の近くなら終了
	if ((actor->GetTargetPosition() - actor->GetPosition()).length() < 10.0f)
	{
		owner->ChangeState(U"eat");
	}
}

void AIMove::OnEnter()
{
}

void AIMove::OnExit()
{
}
