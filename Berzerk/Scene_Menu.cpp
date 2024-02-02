#include "Scene_Menu.h"
#include "Scene_Berzerk.h"
#include <memory>
#include "MusicPlayer.h"

void Scene_Menu::onEnd()
{
	m_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
	MusicPlayer::getInstance().play("menuTheme");
	MusicPlayer::getInstance().setVolume(50);

	// Load the background textures
	m_backgroundTextures.resize(4);
	m_backgroundTextures[0].loadFromFile("../assets/Textures//Menu_Berzerk1.png");
	m_backgroundTextures[1].loadFromFile("../assets/Textures/Menu_Berzerk2.png");
	m_backgroundTextures[2].loadFromFile("../assets/Textures/Menu_Berzerk3.png");
	m_backgroundTextures[3].loadFromFile("../assets/Textures/Menu_Berzerk4.png");

	// Set the background sprite to the first texture
	m_backgroundSprite.setTexture(m_backgroundTextures[0]);

	// Set the interval for the background change
	m_backgroundChangeInterval = sf::seconds(0.5f);
}



void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_menuStrings.push_back("Start Game");
	m_menuStrings.push_back("Credits");
	m_menuStrings.push_back("Exit Game");

	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level1.txt");

	m_menuText.setFont(Assets::getInstance().getFont("main"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);

}

void Scene_Menu::update(sf::Time dt)
{
	if (m_backgroundClock.getElapsedTime() > m_backgroundChangeInterval)
	{
		// Change the background
		static int backgroundIndex = 0;
		backgroundIndex = (backgroundIndex + 1) % m_backgroundTextures.size();
		m_backgroundSprite.setTexture(m_backgroundTextures[backgroundIndex]);

		// Reset the clock
		m_backgroundClock.restart();
	}

	m_entityManager.update();
}

void Scene_Menu::sRender() {

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color selectedOutlineColor(255, 0, 0);
	static const sf::Color normalColor(255, 255, 255);

	// Render the background
	m_game->window().draw(m_backgroundSprite);

	const int CHAR_SIZE = 48;
	sf::Text optionText;
	optionText.setFont(Assets::getInstance().getFont("main"));
	optionText.setCharacterSize(CHAR_SIZE);

	for (size_t i = 0; i < m_menuStrings.size(); ++i) {
		optionText.setString(m_menuStrings[i]);
		optionText.setStyle(sf::Text::Regular);


		if (i == m_menuIndex) {
			optionText.setFillColor(selectedColor);
			optionText.setOutlineColor(selectedOutlineColor);
			optionText.setOutlineThickness(2);
		}
		else {
			optionText.setFillColor(normalColor);
			optionText.setOutlineThickness(0);
		}

		optionText.setOrigin(optionText.getLocalBounds().width / 2.f, 0);
		optionText.setPosition(m_game->window().getSize().x / 2.f, m_game->window().getSize().y * 0.45f + i * 55); // Posicione as opções verticalmente
		m_game->window().draw(optionText);
	}
}

void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuStrings.size() - 1) % m_menuStrings.size();
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			Assets::getInstance().reset();
			m_game->changeScene("PLAY", std::make_shared<Scene_Berzerk>(m_game, m_levelPaths[m_menuIndex]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}
