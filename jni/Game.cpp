#include "Game.h"
#include "../StarEngine/jni/SceneManaging/SceneManager.h"
#include "../StarEngine/jni/Context.h"
#include "../StarEngine/jni/StarEngine.h"
#include "../StarEngine/jni/Helpers/Helpers.h"
#include "scenes/TestScene.h"
#include "scenes/TestScene2.h"

Game::Game()
{
	star::StarEngine::GetInstance();
}

status Game::Initialize(int32 window_width, int32 window_height)
{
	ASSERT(star::StarEngine::GetInstance()->Initialize(window_width, window_height) == STATUS_OK, _T("StarEngine couldn't be initialized!"));

	auto scene1 = new TestScene(_T("TestScene"));
	star::SceneManager::GetInstance()->AddScene(scene1->GetName(),scene1);
	star::SceneManager::GetInstance()->SetActiveScene(_T("TestScene"));

	auto scene2 = new TestScene2(_T("TestScene2"));
	star::SceneManager::GetInstance()->AddScene(scene2->GetName(),scene2);

	star::Logger::GetInstance()->Log(star::LogLevel::Info, _T("Scenes Set and Done"));

	return STATUS_OK;
}

status Game::Update(const star::Context& context)
{
	tstring subTitle(_T("... FPS: "));
	subTitle += star::string_cast<tstring>(star::StarEngine::GetInstance()->GetCurrentFPS());
	star::StarEngine::GetInstance()->SetGameSubTitle(subTitle);
	return star::StarEngine::GetInstance()->Update(context);
}

status Game::Draw()
{
	return star::StarEngine::GetInstance()->Draw();
}

status Game::End()
{
	star::Logger::GetInstance()->Log(star::LogLevel::Info, _T("Game ends."));
	return star::StarEngine::GetInstance()->End();
}
