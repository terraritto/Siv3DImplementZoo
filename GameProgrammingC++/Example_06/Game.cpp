#include "Game.h"
#include <Siv3D.hpp>
#include "Actor.h"
#include "ShaderData.h"

Game::Game()
	: m_isRunning(true)
	, m_actorList()
	, m_pendingActorList()
	, m_updatingActors(false)
{
}

bool Game::initialize()
{
	m_renderer = std::make_shared<Renderer>(shared_from_this());

	if (m_renderer->Initialize() == false)
	{
		m_renderer.reset();
		return false;
	}

	LoadData();

    return true;
}

void Game::runLoop()
{
	while (m_isRunning)
	{
		if (System::Update() == false)
		{
			m_isRunning = false;
			break;
		}

		ClearPrint();

		processInput();
		updateGame();
		generateOutput();
	}
}

void Game::shutdown()
{
	UnloadData();
	if (m_renderer)
	{
		m_renderer->Shutdown();
	}
}

void Game::RemoveActor(std::weak_ptr<Actor> actor)
{
	auto actorPtr = actor.lock();
	if (!actorPtr) { return; }

	auto iter = std::find(m_pendingActorList.begin(), m_pendingActorList.end(), actorPtr);
	if (iter != m_pendingActorList.end())
	{
		std::iter_swap(iter, m_pendingActorList.end() - 1);
		m_pendingActorList.pop_back();
	}

	iter = std::find(m_actorList.begin(), m_actorList.end(), actorPtr);
	if (iter != m_actorList.end())
	{
		std::iter_swap(iter, m_actorList.end() - 1);
		m_actorList.pop_back();
	}
}

void Game::processInput()
{
	m_updatingActors = true;
	for (auto& actor : m_actorList)
	{
		actor->ProcessInput();
	}
	m_updatingActors = false;
}

void Game::updateGame()
{
	double deltaTime = Scene::DeltaTime();

	// actorの更新
	m_updatingActors = true;
	for (auto& actor : m_actorList)
	{
		actor->Update(deltaTime);
	}
	m_updatingActors = false;

	// pendingのactorを移す
	for (auto& actor : m_pendingActorList)
	{
		actor->ComputeWorldTransform();
		m_actorList.emplace_back(actor);
	}
	m_pendingActorList.clear();

	// いらないActorを削除
	while (true)
	{
		auto iter = std::find_if(m_actorList.begin(), m_actorList.end(),
			[](std::shared_ptr<Actor>& data)
			{
				return data->GetState() == Actor::State::Dead;
			}
		);

		if (iter != m_actorList.end())
		{
			m_actorList.erase(std::remove(m_actorList.begin(), m_actorList.end() ,*iter), m_actorList.end());
			continue;
		}

		break;
	}
}

void Game::generateOutput()
{
	m_renderer->Draw();
}

void Game::LoadData()
{
	Size sceneSize = Window::GetState().virtualSize;

	std::shared_ptr<Actor> a = SpawnCastActor<Actor>().lock();
	a->SetPosition({ 0.0f, -2.0f, 0.0f });
	a->SetRotation(Quaternion::RotateY(180_deg));
	a->Initialize();
	std::shared_ptr<MeshComponent> mc = CreateCastComponent<MeshComponent>(a).lock();
	m_renderer->AddMeshComponent(mc);
	mc->SetModel(m_renderer->GetModel(U"example/obj/blacksmith.obj"));
}

void Game::UnloadData()
{
	// actorリソースをクリア
	m_actorList.clear();

	if (m_renderer) { m_renderer->UnloadData(); }
}
