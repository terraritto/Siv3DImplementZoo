#pragma once
#include <Siv3D.hpp>

class Game;
class Component;

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	enum class State
	{
		Active,
		Paused,
		Dead
	};

	Actor(std::weak_ptr<Game> game);
	virtual ~Actor();

	// 初期化
	virtual void Initialize(){}

	// 更新関係
	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);

	// 入力関係
	void ProcessInput();
	virtual void ActorInput();

	// Getter/Stter
	void SetPosition(const Vec3& pos) { m_position = pos; m_recomputeWorldTransform = true; }
	void SetScale(float scale) { m_scale = scale; m_recomputeWorldTransform = true; }
	void SetRotation(Quaternion rotation) { m_rotation = rotation; m_recomputeWorldTransform = true; }
	void SetState(State state) { m_state = state; }

	[[nodiscard]]
	const Vec3 GetPosition() const { return m_position; }

	[[nodiscard]]
	float GetScale() const { return m_scale; }

	[[nodiscard]]
	const Quaternion& GetRotation() const { return m_rotation; }

	[[nodiscard]]
	State GetState() const { return m_state; }

	[[nodiscard]]
	std::weak_ptr<Game> GetGame() { return m_game; }

	void ComputeWorldTransform();

	[[nodiscard]]
	const Mat4x4& GetWorldTransform() const { return m_worldTransform; }

	[[nodiscard]]
	const Vec3 GetForward();

	// コンポーネント関係
	void AddComponent(std::shared_ptr<Component> component);

protected:
	std::weak_ptr<Game> m_game;

private:
	// 状態
	State m_state;

	// Transform
	Mat4x4 m_worldTransform;
	Vec3 m_position;
	float m_scale;
	Quaternion m_rotation;
	bool m_recomputeWorldTransform;

	std::vector<std::shared_ptr<Component>> m_componentList;
};
