#pragma once
#include "Component.h"

class CircleComponent : public Component
{
public:
	CircleComponent(std::weak_ptr<Actor> owner);

	virtual void Update(float deltaTime) override;

	void SetRadius(float radius);

	[[nodiscard]]
	double GetRadius() const;

	[[nodiscard]]
	Vec2 GetCenter() const;

	[[nodiscard]]
	Circle GetCirle() const { return m_circle; }

	[[nodiscard]]
	bool Intersect(std::weak_ptr<CircleComponent> circle);

private:
	Circle m_circle;
};
