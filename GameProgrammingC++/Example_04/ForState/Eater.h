#pragma once
#include "../Actor.h"
#include <vector>

class SpriteComponent;

class Eater : public Actor
{
public:
	// constructor
	Eater(std::weak_ptr<Game> game);

	virtual void Initialize() override;

	void SetTargetPosition(const Float2 pos) { m_targetPosition = pos; }
	const Float2 GetTargetPosition() const { return m_targetPosition; }

private:
	std::weak_ptr<SpriteComponent> m_sprite;

	Float2 m_targetPosition;
};
