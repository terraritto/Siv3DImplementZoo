#include "Eater.h"
#include "../Game.h"
#include "../AIComponent.h"
#include "AIFind.h"
#include "AIMove.h"
#include "AIEat.h"
#include "AIEnd.h"

Eater::Eater(std::weak_ptr<Game> game)
	: Actor(game)
{
}

void Eater::Initialize()
{
	Size sceneSize = Window::GetState().virtualSize;
	auto game = m_game.lock();
	auto owner = this->shared_from_this();

	// 初期位置を中心に
	SetPosition(Vec2(sceneSize) / 2.0f);

	// スプライト用のComponentの作成
	m_sprite = game->SpawnSprite<SpriteComponent>(owner).lock();

	// Textureを設定
	auto tex = game->GetEmojoTexture(U"🐴");
	m_sprite.lock()->SetTexture(tex);

	// AIの生成
	auto ai = CreateCastComponent<AIComponent>(owner).lock();

	// Find/Move/Eat/End
	ai->RegisterState(std::make_shared<AIFind>(ai));
	ai->RegisterState(std::make_shared<AIMove>(ai));
	ai->RegisterState(std::make_shared<AIEat>(ai));
	ai->RegisterState(std::make_shared<AIEnd>(ai));

	ai->ChangeState(U"find");
	
}
