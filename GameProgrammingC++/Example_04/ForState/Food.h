#pragma once
#include "../Actor.h"
#include <vector>

class SpriteComponent;

class Food : public Actor
{
public:
	// constructor
	Food(std::weak_ptr<Game> game);

	virtual void Initialize() override;

private:
	std::weak_ptr<SpriteComponent> m_sprite;
};
