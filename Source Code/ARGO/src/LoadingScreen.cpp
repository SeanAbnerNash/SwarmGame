#include "stdafx.h"
#include "LoadingScreen.h"
#include "..\include\LoadingScreen.h"

LoadingScreen::LoadingScreen(RenderSystem& t_renderSystem) :
	m_renderSystem{ t_renderSystem }
{
}

LoadingScreen::~LoadingScreen()
{
}

void LoadingScreen::update(float t_deltaTime)
{
	if (SDL_GetTicks() - m_timeSinceLastTextUpdate > TIME_PER_TEXT_CHANGE)
	{
		if (m_loadingString != "Loading   .   .   .")
		{
			m_loadingString += "   .";
		}
		else
		{
			m_loadingString = "Loading";
		}
		static_cast<TextComponent*>(m_loadingText.getComponent(ComponentType::Text))->setText(m_loadingString);
		m_timeSinceLastTextUpdate = SDL_GetTicks();
	}
}

void LoadingScreen::render(SDL_Renderer* t_renderer)
{
	m_renderSystem.render(t_renderer, m_background);
	m_renderSystem.render(t_renderer, m_loadingText);
}

void LoadingScreen::reset()
{
}

void LoadingScreen::initialise(SDL_Renderer* t_renderer)
{
	m_loadingString = "Loading";
	m_timeSinceLastTextUpdate = SDL_GetTicks();

	m_background.addComponent(new VisualComponent("Menu_Background.png", t_renderer));
	VisualComponent* visComp = static_cast<VisualComponent*>(m_background.getComponent(ComponentType::Visual));
	visComp->setStaticPosition(true);
	m_background.addComponent(new TransformComponent(0, 0));
	static_cast<TransformComponent*>(m_background.getComponent(ComponentType::Transform))->setAlwaysOnScreen(true);


	m_loadingText.addComponent(new TextComponent("ariblk.ttf", t_renderer, Utilities::MEDIUM_FONT, true, m_loadingString, 255, 255, 0));
 	TextComponent* textComp = static_cast<TextComponent*>(m_loadingText.getComponent(ComponentType::Text));
	glm::vec2 textSize = glm::vec2(textComp->getWidth(), textComp->getHeight());
	m_loadingText.addComponent(new TransformComponent(glm::vec2(Utilities::SCREEN_WIDTH / 2.0f - textSize.x / 2.0f, Utilities::SCREEN_HEIGHT / 2.0f - textSize.y / 2.0f)));
	static_cast<TransformComponent*>(m_loadingText.getComponent(ComponentType::Transform))->setAlwaysOnScreen(true);

 }
