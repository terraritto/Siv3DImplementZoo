#pragma once
#include "GameTreeBaseComponent.h"

class AlphaBetaComponent : public GameTreeBaseComponent
{
public:
	using GameTreeBaseComponent::GameTreeBaseComponent;

	// 決定
	Array<Array<GTState>> Decide() override;

protected:
	float MinPlayer(const std::shared_ptr<GTNode> node, float alpha, float beta);
	float MaxPlayer(const std::shared_ptr<GTNode> node, float alpha, float beta);
};
