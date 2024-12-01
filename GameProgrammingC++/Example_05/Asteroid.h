#pragma once
#include "Actor.h"
#include "CircleComponent.h"

class Asteroid : public Actor
{
public:
	Asteroid(std::weak_ptr<Game> game);
	~Asteroid();

	virtual void Initialize() override;

	[[nodiscard]]
	std::weak_ptr<CircleComponent> GetCircle() { return m_circle; }

private:
	std::weak_ptr<CircleComponent> m_circle;
};
