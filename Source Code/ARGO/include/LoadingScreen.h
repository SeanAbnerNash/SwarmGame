#pragma once
#include "EventManager.h"
#include "Controller.h"
#include "InputSystem.h"
#include "CommandSystem.h"
#include "RenderSystem.h"

class LoadingScreen
{
public:
	LoadingScreen(RenderSystem& t_renderSystem);
	~LoadingScreen();

	void update(float t_deltaTime);
	void render(SDL_Renderer* t_renderer);
	void reset();
	void initialise(SDL_Renderer* t_renderer);
private:

	Entity m_background;
	Entity m_loadingText;

	std::string m_loadingString;
	int m_timeSinceLastTextUpdate;
	const int TIME_PER_TEXT_CHANGE = 350;

	RenderSystem& m_renderSystem;
 };

