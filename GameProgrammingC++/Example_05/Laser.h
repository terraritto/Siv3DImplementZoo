#pragma once
#include "Actor.h"
#include "CircleComponent.h"

class Laser : public Actor
{
public:
	Laser(std::weak_ptr<Game> game);

	virtual void Initialize() override;
	virtual void UpdateActor(float deltaTime) override;

private:
	std::weak_ptr<CircleComponent> m_circle;
	Timer m_timer;
};
