#pragma once
#include "GameTreeBaseComponent.h"

class MinMaxComponent : public GameTreeBaseComponent
{
public:
	using GameTreeBaseComponent::GameTreeBaseComponent;

	// 決定
	Array<Array<GTState>> Decide() override;

protected:
	float MinPlayer(const std::shared_ptr<GTNode> node);
	float MaxPlayer(const std::shared_ptr<GTNode> node);
};
