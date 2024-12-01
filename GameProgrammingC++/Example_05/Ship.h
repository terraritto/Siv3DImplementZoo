#pragma once
#include "Actor.h"

class Ship : public Actor
{
public:
	Ship(std::weak_ptr<Game> game);

	// 更新関係
	virtual void UpdateActor(float deltaTime) override;

	virtual void Initialize() override;
	virtual void ActorInput() override;

protected:
	Timer m_laserTimer;
};
