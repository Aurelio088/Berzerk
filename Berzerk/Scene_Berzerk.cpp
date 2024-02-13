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
	auto posEnemy = m_worldView.getSize();

	// spawn player in middle of first row
	pos.x = 0.f - 20.f;
	pos.y -= 600.f - 260.f;

	spawnPlayer(pos);

	// spawn enemy
	posEnemy.x = 500.f - 20.f;
	posEnemy.y -= 600.f - 260.f;


	//spawnEnemy(posEnemy);


	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(30);

}


void Scene_Berzerk::init(const std::string& path) {
}

void Scene_Berzerk::sMovement(sf::Time dt) {
	playerMovement();

	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		//if (e->hasComponent<CInput>())
		//	continue; // player is moved in playerMovement
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			/*tfm.pos += tfm.vel * dt.asSeconds();
			tfm.angle += tfm.angVel * dt.asSeconds();*/

			tfm.prevPos = tfm.pos;					// Salva a posição atual como anterior (antiga).
			tfm.pos += tfm.vel * dt.asSeconds();	// Atualiza a posição atual para a nova.
		}
	}
}


void Scene_Berzerk::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");

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

	// player movement
	sf::Vector2f pv{ 0.f, 0.f };
	auto& animation = m_player->getComponent<CAnimation>().animation;
	auto& pInput = m_player->getComponent<CInput>();

	// Diagonal movements
	if (pInput.up && pInput.left) {
		pv.x -= 1;
		pv.y -= 1;

		if (animation.getName() != "left")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;
		animation.play();
	}
	else if (pInput.up && pInput.right) {
		pv.x += 1;
		pv.y -= 1;

		if (animation.getName() != "right")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;
		animation.play();
	}
	else if (pInput.down && pInput.left) {
		pv.x -= 1;
		pv.y += 1;

		if (animation.getName() != "left")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;
		animation.play();
	}
	else if (pInput.down && pInput.right) {
		pv.x += 1;
		pv.y += 1;

		if (animation.getName() != "right")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;
		animation.play();
	}

	// Single movements
	else if (pInput.left) {
		pv.x -= 1;

		if (animation.getName() != "left")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;
		animation.play();
	}
	else if (pInput.right) {
		pv.x += 1;

		if (animation.getName() != "right")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;
		animation.play();
	}
	else if (pInput.up) {
		pv.y -= 1;

		if (animation.getName() != "up")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("up")).animation;
		animation.play();
	}
	else if (pInput.down) {
		pv.y += 1;

		if (animation.getName() != "down")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("down")).animation;
		animation.play();
	}

	pv = normalize(pv);
	m_player->getComponent<CTransform>().vel = m_config.playerSpeed * pv;
}


void Scene_Berzerk::sRender() {
	m_game->window().setView(m_worldView);

	// Draw black background
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(m_game->window().getSize().x, m_game->window().getSize().y));
	background.setFillColor(sf::Color::Black);
	m_game->window().draw(background);

	// draw bkg first
	for (auto e : m_entityManager.getEntities("bkg")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	// Set the view to the center of the player
	auto& pPos = m_player->getComponent<CTransform>().pos;
	float centerX = m_game->window().getSize().x / 2;
	float centerY = m_game->window().getSize().y / 2;
	sf::View view = m_game->window().getView();
	view.setCenter(centerX, centerY);
	m_game->window().setView(view);

	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CSprite>())
		{
			auto& sprite = e->getComponent<CSprite>().sprite;
			if (e->hasComponent<CTransform>()) {
				auto& tfm = e->getComponent<CTransform>();
				//sprite.setPosition(tfm.pos);

			}
			m_game->window().draw(sprite);
		}

		if (e->hasComponent<CAnimation>())
		{
			auto& anim = e->getComponent<CAnimation>().animation;
			auto& tfm = e->getComponent<CTransform>();
			anim.getSprite().setPosition(tfm.pos);
			anim.getSprite().setRotation(tfm.angle);
			m_game->window().draw(anim.getSprite());
		}

		if (m_drawAABB) {
			if (e->hasComponent<CBoundingBox>()) {
				auto box = e->getComponent<CBoundingBox>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
				centerOrigin(rect);
				rect.setPosition(e->getComponent<CTransform>().pos + box.offset);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color{ 0, 255, 0 });
				rect.setOutlineThickness(0.5f);
				m_game->window().draw(rect);
			}
		}
	}

	// draw grid
	sf::VertexArray lines(sf::Lines);
	sf::Text gridText;
	gridText.setFont(Assets::getInstance().getFont("Arial"));
	gridText.setCharacterSize(10);

	if (m_drawGrid) {
		float left = view.getCenter().x - view.getSize().x / 2.f;
		float right = left + view.getSize().x;
		float top = view.getCenter().y - view.getSize().y / 2.f;
		float bot = top + view.getSize().y;

		// align grid to grid size
		int nCols = static_cast<int>(view.getSize().x) / m_gridSize.x;
		int nRows = static_cast<int>(view.getSize().y) / m_gridSize.y;

		// row and col # of bot left
		const int ROW0 = 600;
		int firstCol = static_cast<int>(left) / static_cast<int>(m_gridSize.x);
		int lastRow = static_cast<int>(bot) / static_cast<int>(m_gridSize.y);

		lines.clear();

		// vertical lines
		for (int x{ 0 }; x <= nCols; ++x) {
			lines.append(sf::Vector2f((firstCol + x) * m_gridSize.x, top));
			lines.append(sf::Vector2f((firstCol + x) * m_gridSize.x, bot));
		}

		// horizontal lines
		for (int y{ 0 }; y <= nRows; ++y) {
			lines.append(sf::Vector2f(left, ROW0 - (lastRow - y) * m_gridSize.y));
			lines.append(sf::Vector2f(right, ROW0 - (lastRow - y) * m_gridSize.y));
		}

		// grid coordinates
		for (int x{ 0 }; x <= nCols; ++x) {
			for (int y{ 0 }; y <= nRows; ++y) {
				std::string label = std::string(
					"(" + std::to_string(firstCol + x) + ", " + std::to_string(lastRow - y) + ")");
				gridText.setString(label);
				gridText.setPosition((x + firstCol) * m_gridSize.x, ROW0 - (lastRow - y + 1) * m_gridSize.y);
				m_game->window().draw(gridText);
			}
		}

		m_game->window().draw(lines);
	}


	// draw score
	//m_game->window().draw(m_scoreText);

	// draw lives
	//m_game->window().draw(m_livesText);

	// draw win text
	//m_game->window().draw(m_endText);

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
		if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
	}

	// Key release
	else if (action.type() == "END") {
		// Player control
		if (action.name() == "LEFT") {
			m_player->getComponent<CInput>().left = false;
			if (m_player->hasComponent<CAnimation>()) {
				auto& animation = m_player->getComponent<CAnimation>().animation;
				animation.stop();
			}
		}
		else if (action.name() == "RIGHT") {
			m_player->getComponent<CInput>().right = false;
			if (m_player->hasComponent<CAnimation>()) {
				auto& animation = m_player->getComponent<CAnimation>().animation;
				animation.stop();
			}
		}
		else if (action.name() == "UP") {
			m_player->getComponent<CInput>().up = false;
			if (m_player->hasComponent<CAnimation>()) {
				auto& animation = m_player->getComponent<CAnimation>().animation;
				animation.stop();
			}
		}
		else if (action.name() == "DOWN") {
			m_player->getComponent<CInput>().down = false;
			if (m_player->hasComponent<CAnimation>()) {
				auto& animation = m_player->getComponent<CAnimation>().animation;
				animation.stop();
			}
		}
	}


	// on Key Release
	// the frog can only go in one direction at a time, no angles
	// use a bitset and exclusive setting.
	/*else if (action.type() == "END" && (action.name() == "LEFT" || action.name() == "RIGHT" || action.name() == "UP" ||
		action.name() == "DOWN")) {
		m_player->getComponent<CInput>().dir = 0;
	}*/
}


void Scene_Berzerk::spawnPlayer(sf::Vector2f pos) {

	m_player = m_entityManager.addEntity("player");

	sf::Vector2f boundingBoxOffset{ 0.f, 10.f }; // Moving the bounding box down a bit
	m_player->setBoundingBoxOffset(boundingBoxOffset);

	m_player->addComponent<CTransform>(pos);
	m_player->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	m_player->addComponent<CInput>();
	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
	m_player->addComponent<CState>("grounded");
}

// spawn Enemy
void Scene_Berzerk::spawnEnemy(sf::Vector2f pos) {
	auto enemy = m_entityManager.addEntity("enemy");
	enemy->addComponent<CTransform>(pos);
	enemy->addComponent<CBoundingBox>(sf::Vector2f(15.f, 15.f));
	enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("left"));
	enemy->addComponent<CState>("grounded");
}


sf::FloatRect Scene_Berzerk::getViewBounds() {
	return sf::FloatRect();
}

void Scene_Berzerk::sCollisions() {

	adjustPlayerPosition();

	if (m_player->getComponent<CState>().state == "dead") {
		return;
	}

	// Collisions player vs walls
	auto players = m_entityManager.getEntities("player");
	auto tiles = m_entityManager.getEntities("tile");

	// Collision player vs bricks (tiles)
	for (auto& player : players) {
		for (auto& tile : tiles) {
			if (checkCollision(*player, *tile)) {
				auto& transform = player->getComponent<CTransform>();
				auto& box = player->getComponent<CBoundingBox>();
				auto& tileBox = tile->getComponent<CBoundingBox>();
				sf::FloatRect playerRect(
					transform.pos.x - box.size.x / 2.f,
					transform.pos.y - box.size.y / 2.f,
					box.size.x, box.size.y);
				sf::FloatRect tileRect(
					tile->getComponent<CTransform>().pos.x - tileBox.size.x / 2.f,
					tile->getComponent<CTransform>().pos.y - tileBox.size.y / 2.f,
					tileBox.size.x, tileBox.size.y);
				sf::Vector2f depth;
				/*if (playerRect.intersects(tileRect, depth)) {
					if (std::abs(depth.x) < std::abs(depth.y)) {
						transform.pos.x += depth.x;
					}
					else {
						transform.pos.y += depth.y;
					}
				}*/
			}
		}
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
		else if (token == "Tile") {
			std::string name;
			sf::Vector2f pos;
			sf::IntRect rect;
			std::string collide;
			config >> name >> pos.x >> pos.y >> rect.left >> rect.top >> rect.width >> rect.height >> collide;
			auto e = m_entityManager.addEntity("Tile");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name), rect).sprite;
			sprite.setTextureRect(rect);

			pos.x += (rect.width / 2.0f);
			pos.y += (rect.height / 2.0f);

			sprite.setPosition(pos);


			if (collide == "yes") {
				e->addComponent<CBoundingBox>(sf::Vector2f(40.f, 40.f));
			}
		}
		else if (token[0] == '#') {
			std::string tmp;
			std::getline(config, tmp);
			std::cout << tmp << "\n";
		}

		config >> token;
	}
	config.close();
}


