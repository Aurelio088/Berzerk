//
// Created by David Burchill on 2023-09-27.
//

#include <fstream>
#include <iostream>

#include "Scene_Berzerk.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include <random>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}


Scene_Berzerk::Scene_Berzerk(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_worldView(gameEngine->window().getDefaultView()) {
	loadLevel(levelPath);
	registerActions();

	auto pos = m_worldView.getSize();

	// spawn frog in middle of first row
	pos.x = 0.f - 20.f;
	pos.y -= 600.f - 260.f;


	spawnPlayer(pos);

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(50);

}


void Scene_Berzerk::init(const std::string& path) {
}

void Scene_Berzerk::sMovement(sf::Time dt) {
	playerMovement();

	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CInput>())
			continue; // player is moved in playerMovement
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.pos += tfm.vel * dt.asSeconds();
			tfm.angle += tfm.angVel * dt.asSeconds();

			// Reset entity position if it goes off-screen (using tags to identify entities)
			if (e->getTag() == "moveLeft" && tfm.pos.x < -50.f) {
				// Reset the entity's position off-screen to the right or adjust as needed
				tfm.pos.x = 480.f + 50.f; // Assuming 480 is the width of your window
			}

			if (e->getTag() == "moveRight" && tfm.pos.x > 480.f + 50.f) {
				// Reset the car's position off-screen to the left or adjust as needed
				tfm.pos.x = 0.f - 50.f; // Assuming -50 is enough to be off-screen
			}

			if (e->getTag() == "tree" && tfm.pos.x > 480.f + 100.f) {
				// Reset the car's position off-screen to the right or adjust as needed
				tfm.pos.x = 0.f - 60.f; // Assuming 480 is the width of your window
			}
		}
	}
}


void Scene_Berzerk::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
}


void Scene_Berzerk::onEnd() {
	m_game->changeScene("MENU", nullptr, false);
}

void Scene_Berzerk::playerMovement() {
	// no movement if player is dead
	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead")
		return;

	auto& dir = m_player->getComponent<CInput>().dir;
	auto& pos = m_player->getComponent<CTransform>().pos;

	if (dir & CInput::UP) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("up"));
		pos.y -= 5.f;
	}
	if (dir & CInput::DOWN) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("down"));
		pos.y += 5.f;
	}

	if (dir & CInput::LEFT) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("left"));
		pos.x -= 5.f;
	}

	if (dir & CInput::RIGHT) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
		pos.x += 5.f;
	}

	if (dir != 0) {
		SoundPlayer::getInstance().play("hop", m_player->getComponent<CTransform>().pos);
		dir = 0;
	}
}


void Scene_Berzerk::sRender() {
	m_game->window().setView(m_worldView);

	// draw bkg first
	for (auto e : m_entityManager.getEntities("bkg")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	for (auto& e : m_entityManager.getEntities()) {
		if (!e->hasComponent<CAnimation>())
			continue;

		// Draw Sprite
		auto& anim = e->getComponent<CAnimation>().animation;
		auto& tfm = e->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		anim.getSprite().setRotation(tfm.angle);
		m_game->window().draw(anim.getSprite());

		if (m_drawAABB) {
			if (e->hasComponent<CBoundingBox>()) {
				auto box = e->getComponent<CBoundingBox>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
				centerOrigin(rect);
				rect.setPosition(e->getComponent<CTransform>().pos);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color{ 0, 255, 0 });
				rect.setOutlineThickness(2.f);
				m_game->window().draw(rect);
			}
		}
	}

	// draw score
	//m_game->window().draw(m_scoreText);

	// draw lives
	//m_game->window().draw(m_livesText);

	// draw win text
	m_game->window().draw(m_endText);
}


void Scene_Berzerk::update(sf::Time dt) {
	sUpdate(dt);
}

void Scene_Berzerk::sDoAction(const Command& action) {
	// On Key Press
	if (action.type() == "START") {
		if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
		else if (action.name() == "QUIT") { m_game->quitLevel(); }
		else if (action.name() == "BACK") { m_game->backLevel(); }

		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

		// Player control
		if (action.name() == "LEFT") { m_player->getComponent<CInput>().dir = CInput::LEFT; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().dir = CInput::RIGHT; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().dir = CInput::UP; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().dir = CInput::DOWN; }
	}
	// on Key Release
	// the frog can only go in one direction at a time, no angles
	// use a bitset and exclusive setting.
	else if (action.type() == "END" && (action.name() == "LEFT" || action.name() == "RIGHT" || action.name() == "UP" ||
		action.name() == "DOWN")) {
		m_player->getComponent<CInput>().dir = 0;
	}
}


void Scene_Berzerk::spawnPlayer(sf::Vector2f pos) {
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(pos);
	m_player->addComponent<CBoundingBox>(sf::Vector2f(15.f, 15.f));
	m_player->addComponent<CInput>();
	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
	m_player->addComponent<CState>("grounded");
}


sf::FloatRect Scene_Berzerk::getViewBounds() {
	return sf::FloatRect();
}

void Scene_Berzerk::sCollisions() {

	adjustPlayerPosition();

	if (m_player->getComponent<CState>().state == "dead") {
		return;
	}


}




bool Scene_Berzerk::checkCollision(Entity& entity1, Entity& entity2) {
	if (entity1.hasComponent<CBoundingBox>() && entity2.hasComponent<CBoundingBox>()) {
		auto& box1 = entity1.getComponent<CBoundingBox>();
		auto& box2 = entity2.getComponent<CBoundingBox>();

		sf::FloatRect rect1(
			entity1.getComponent<CTransform>().pos.x - box1.size.x / 2.f,
			entity1.getComponent<CTransform>().pos.y - box1.size.y / 2.f,
			box1.size.x, box1.size.y);

		sf::FloatRect rect2(
			entity2.getComponent<CTransform>().pos.x - box2.size.x / 2.f,
			entity2.getComponent<CTransform>().pos.y - box2.size.y / 2.f,
			box2.size.x, box2.size.y);

		return rect1.intersects(rect2);
	}

	return false;
}

void Scene_Berzerk::onPlayerCollision() {
	SoundPlayer::getInstance().play("death", m_player->getComponent<CTransform>().pos);
	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("die"));

	// Deccrement life
	Assets::getInstance().decrementLife(1);

	// Decrement score
	Assets::getInstance().decrementScore(10);


	updateLivesText();

	if (Assets::getInstance().getLife() == 0) {
		m_player->addComponent<CState>().state = "dead";
		MusicPlayer::getInstance().stop();
		if (!m_player->getComponent<CAnimation>().animation.hasEnded()) {
			onEnd();
		}
	}
	else {
		// Respawn player
		m_player->addComponent<CState>().state = "dead";
	}
}

void Scene_Berzerk::respawnPlayer() {
	auto pos = m_worldView.getSize();
	pos.x = 0.f - 20.f;
	pos.y -= 600.f - 340.f;

	m_player->getComponent<CTransform>().pos = pos;

	// set the player to initial state 
	auto& playerState = m_player->getComponent<CState>().state;
	playerState = "grounded";

	// set the player to initial animation
	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
}


void Scene_Berzerk::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();
	m_entityManager.update();

	if (m_isPaused)
		return;

	updateScoreText(); // update score text
	updateLivesText();

	sAnimation(dt);
	sMovement(dt);
	adjustPlayerPosition();
	checkPlayerState();
	sCollisions();
}

void Scene_Berzerk::updateScoreText() {
	m_font = Assets::getInstance().getFont("Arcade");
	m_scoreText.setString("Score  " + std::to_string(Assets::getInstance().getScore()));

	// Setting the score text properties
	m_scoreText.setPosition(20.f, 15.f);
	m_scoreText.setFont(m_font);
	m_scoreText.setCharacterSize(30);
	m_scoreText.setFillColor(sf::Color::Green);
}

void Scene_Berzerk::updateLivesText() {
	m_font = Assets::getInstance().getFont("Arcade");
	m_livesText.setString("Lives  " + std::to_string(Assets::getInstance().getLife()));
	// Setting the lives text properties
	m_livesText.setPosition(320.f, 15.f);
	m_livesText.setFont(m_font);
	m_livesText.setCharacterSize(30);
	m_livesText.setFillColor(sf::Color::Red);
}

// Message of winning
void Scene_Berzerk::winningMessage() {
	m_isPaused = true;

	m_font = Assets::getInstance().getFont("Arcade");
	m_endText.setString("You Win");
	m_endText.setPosition(150.f, 300.f);
	m_endText.setFont(m_font);
	m_endText.setCharacterSize(50);
	m_endText.setFillColor(sf::Color::Red);
}


void Scene_Berzerk::sAnimation(sf::Time dt) {
	auto list = m_entityManager.getEntities();
	for (auto e : m_entityManager.getEntities()) {
		// update all animations
		if (e->hasComponent<CAnimation>()) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);
			// do nothing if animation has ended
		}
	}
}


void Scene_Berzerk::adjustPlayerPosition() {
	auto center = m_worldView.getCenter();
	sf::Vector2f viewHalfSize = m_worldView.getSize() / 2.f;


	auto left = center.x - viewHalfSize.x;
	auto right = center.x + viewHalfSize.x;
	auto top = center.y - viewHalfSize.y;
	auto bot = center.y + viewHalfSize.y;

	auto& player_pos = m_player->getComponent<CTransform>().pos;
	auto halfSize = sf::Vector2f{ 20, 20 };
	// keep player in bounds
	player_pos.x = std::max(player_pos.x, left + halfSize.x);
	player_pos.x = std::min(player_pos.x, right - halfSize.x);
	player_pos.y = std::max(player_pos.y, top + halfSize.y);
	player_pos.y = std::min(player_pos.y, bot - halfSize.y);
}

void Scene_Berzerk::checkPlayerState() {
	auto& state = m_player->getComponent<CState>().state;
	if (state == "dead" && m_player->getComponent<CAnimation>().animation.hasEnded()) {
		respawnPlayer();
	}
}

void Scene_Berzerk::loadLevel(const std::string& path) {
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "Bkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("bkg");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token[0] == '#') {
			std::cout << token;
		}

		config >> token;
	}

	config.close();
}
