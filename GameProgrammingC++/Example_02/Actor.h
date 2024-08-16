#pragma once
#include <Siv3D.hpp>

class Game;
class Component;

class Actor
{
public:
	enum class State
	{
		Active,
		Paused,
		Dead
	};

	Actor(Game* game);
	virtual ~Actor();

	// 更新関係
	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);

	// Getter/Stter
	void SetPosition(const Vector2D<float>& pos) { m_position = pos; }
	void SetScale(float scale) { m_scale = scale; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetState(State state) { m_state = state; }

	[[nodiscard]]
	const Vector2D<float>& GetPosition() const { return m_position; }

	[[nodiscard]]
	float GetScale() const { return m_scale; }

	[[nodiscard]]
	float GetRotation() const { return m_rotation; }

	[[nodiscard]]
	State GetState() const { return m_state; }

	[[nodiscard]]
	Game* GetGame() { return m_game; }

	// コンポーネント関係
	void AddComponent(std::shared_ptr<Component> component);

private:
	// 状態
	State m_state;

	// Transform
	s3d::Vector2D<float> m_position;
	float m_scale;
	float m_rotation;

	std::vector<std::shared_ptr<Component>> m_componentList;
	Game* m_game;
};
