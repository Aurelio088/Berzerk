#include "Scene_Credits.h"
#include "Scene_Menu.h"
#include "MusicPlayer.h"

Scene_Credits::Scene_Credits(GameEngine* gameEngine) : Scene(gameEngine) {
    
    init();
    MusicPlayer::getInstance().play("menuTheme");
    MusicPlayer::getInstance().setVolume(30);

    // Load the background textures
    m_backgroundTextures.resize(4);
    m_backgroundTextures[0].loadFromFile("../assets/Textures//Menu_Berzerk1.png");

    // Set the background sprite to the first texture
    m_backgroundSprite.setTexture(m_backgroundTextures[0]);

    // Set the interval for the background change
    m_backgroundChangeInterval = sf::seconds(0.5f);
}

void Scene_Credits::onEnd()
{
}

void Scene_Credits::init() {

    registerAction(sf::Keyboard::BackSpace, "BACK_TO_MENU");

    m_menuText.setFont(Assets::getInstance().getFont("main"));
    const size_t CHAR_SIZE{ 64 };
    m_menuText.setCharacterSize(CHAR_SIZE);
}

void Scene_Credits::update(sf::Time dt) {
}

void Scene_Credits::sRender() {

    // Render the background
    m_game->window().draw(m_backgroundSprite);

    sf::Text creditText;
    creditText.setFont(Assets::getInstance().getFont("main"));
    creditText.setString("Aurelio Rodrigues\n\nNBCC - Moncton\nGex 2024");
    creditText.setCharacterSize(48);
    creditText.setFillColor(sf::Color::White);

    creditText.setOutlineColor(sf::Color::Red);
    creditText.setOutlineThickness(2);

    sf::FloatRect textBounds = creditText.getLocalBounds();
    creditText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    creditText.setPosition(m_game->window().getView().getSize().x / 2, m_game->window().getView().getSize().y*2.5 / 4);

    m_game->window().draw(creditText);
}

void Scene_Credits::sDoAction(const Command& action) {
    if (action.type() == "START" && action.name() == "BACK_TO_MENU") {
        m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
    }
}