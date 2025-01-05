#pragma once
#include "MoveComponent.h"

class InputComponent : public MoveComponent
{
public:
	InputComponent(std::weak_ptr<Actor> owner);

	virtual void ProcessInput() override;

	// setter
	void SetMaxAngularSpeed(float speed) { m_maxAngularSpeed = speed; }
	void SetMaxForwardSpeed(float speed) { m_maxForwardSpeed = speed; }
	void SetForwardKey(s3d::Input key) { m_forwardKey = key; }
	void SetBackKey(s3d::Input key) { m_backKey = key; }
	void SetClockwiseKey(s3d::Input key) { m_clockwiseKey = key; }
	void SetCounterClockwiseKey(s3d::Input key) { m_counterClockwiseKey = key; }

	[[nodiscard]]
	float GetMaxAngularSpeed() const { return m_maxAngularSpeed; }

	[[nodiscard]]
	float GetMaxForwardSpeed() const { return m_maxForwardSpeed; }

	[[nodiscard]]
	s3d::Input GetForwardKey() const { return m_forwardKey; }

	[[nodiscard]]
	s3d::Input GetBackKey() const { return m_backKey; }

	[[nodiscard]]
	s3d::Input GetClockwiseKey() const { return m_clockwiseKey; }

	[[nodiscard]]
	s3d::Input GetCounterClockwiseKey() const { return m_counterClockwiseKey; }

protected:
	float m_maxAngularSpeed;
	float m_maxForwardSpeed;

	// 前進と後退のキー
	s3d::Input m_forwardKey;
	s3d::Input m_backKey;

	// 時計と反時計周りのキー
	s3d::Input m_clockwiseKey;
	s3d::Input m_counterClockwiseKey;
};
