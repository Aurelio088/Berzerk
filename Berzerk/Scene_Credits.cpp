//#include "Scene_Credits.h"
//#include "Scene_Menu.h"
//#include "MusicPlayer.h"
//
//Scene_Credits::Scene_Credits(GameEngine* gameEngine) : Scene(gameEngine) {
//    
//    init();
//    MusicPlayer::getInstance().play("menuTheme");
//    MusicPlayer::getInstance().setVolume(30);
//
//    // Load the background textures
//    m_backgroundTextures.resize(4);
//    m_backgroundTextures[0].loadFromFile("../assets/Textures//Menu_Berzerk1.png");
//
//    // Set the background sprite to the first texture
//    m_backgroundSprite.setTexture(m_backgroundTextures[0]);
//
//    // Set the interval for the background change
//    m_backgroundChangeInterval = sf::seconds(0.5f);
//
//}
//
//void Scene_Credits::init() {
//
//    registerAction(sf::Keyboard::BackSpace, "BACK_TO_MENU");
//
//    m_menuText.setFont(Assets::getInstance().getFont("main"));
//    const size_t CHAR_SIZE{ 64 };
//    m_menuText.setCharacterSize(CHAR_SIZE);
//}
//
//void Scene_Credits::update(sf::Time dt) {
//    // Lógica de atualização dos créditos (se necessário)
//}
//
//void Scene_Credits::sRender() {
//
//    // Render the background
//    m_game->window().draw(m_backgroundSprite);
//
//    m_creditText.setFont(m_font);
//    m_creditText.setString("Aurelio Rodrigues\nNBCC - Moncton\n\n\nGex 2024");
//    m_creditText.setCharacterSize(48);
//    m_creditText.setFillColor(sf::Color::White);
//    m_creditText.setPosition(100, 100);
//
//    //m_game->window().clear(sf::Color::Black);
//    m_game->window().draw(m_creditText);
//    //m_game->window().display();
//}
//
//void Scene_Credits::sDoAction(const Command& action) {
//    if (action.type() == "START" && action.name() == "BACK_TO_MENU") {
//        // Faz a transição de volta para a cena do menu
//        m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
//    }
//}