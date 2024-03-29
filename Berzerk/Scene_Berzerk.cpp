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

	// spawn player 
	auto pos = m_worldView.getSize();
	pos.x = 0.f - 20.f;
	pos.y -= 600.f - 260.f;

	spawnPlayer(pos);

	// spawn enemy 1 (Grid position 17, 2)
	sf::Vector2f enemy1Position(17, 2);

	// Set the center of the grid cell
	float xCenter = enemy1Position.x * m_gridSize.x + m_gridSize.x / 2.0f;
	float yCenter = m_game->window().getSize().y - (enemy1Position.y * m_gridSize.y + m_gridSize.y / 2.0f);

	// Spawn the enemy
	spawnEnemy(sf::Vector2f(xCenter, yCenter), sf::Vector2f(-50.0f, 0.0f), 2.0f, 17.0f);

	// spawn enemy 2 (Grid position 7, 12)
	sf::Vector2f enemy2Position(7, 12);

	// Calculate the center of the grid cell
	float newXCenter = enemy2Position.x * m_gridSize.x + m_gridSize.x / 2.0f;
	float newYCenter = m_game->window().getSize().y - (enemy2Position.y * m_gridSize.y + m_gridSize.y / 2.0f);

	spawnEnemy(sf::Vector2f(newXCenter, newYCenter), sf::Vector2f(50.0f, 0.0f), 7.0f, 17.0f);

	// spawn dragonSpare
	spawnDragonSpear(sf::Vector2f(12 * m_gridSize.x, (600 - 7 * m_gridSize.y) - m_gridSize.y / 2));

	// spawn power up (position 18, 2)
	spawnPowerUp(sf::Vector2f(18 * m_gridSize.x, (600 - 2 * m_gridSize.y) - m_gridSize.y / 2));

	//MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(30);

}

void Scene_Berzerk::init(const std::string& path) {
}

void Scene_Berzerk::sMovement(sf::Time dt) {
	playerMovement();
	enemyMovement();

	// move all objects
	for (auto& e : m_entityManager.getEntities()) {

		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.prevPos = tfm.pos;
			tfm.pos += tfm.vel * dt.asSeconds();
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
	auto& pPowerUps = m_player->getComponent<CPowerUps>();

	// Diagonal movements
	if (pInput.up && pInput.left) {
		pv.x -= 1;
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.up && pInput.right) {
		pv.x += 1;
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down && pInput.left) {
		pv.x -= 1;
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down && pInput.right) {
		pv.x += 1;
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}

	// Single movements
	else if (pInput.left) {
		pv.x -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.right) {
		pv.x += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.up) {
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearUp" : "up";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down) {
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearDown" : "down";

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}

	pv = normalize(pv);
	m_player->getComponent<CTransform>().vel = m_config.playerSpeed * pv;
}

void Scene_Berzerk::enemyMovement() {
	auto& enemies = m_entityManager.getEntities("enemy");

	for (auto& enemy : enemies)
	{
		auto& enemyTransform = enemy->getComponent<CTransform>();
		auto& enemyScript = enemy->getComponent<CScript>();
		sf::Vector2f currentPos = enemyTransform.pos;

		if (currentPos.x >= enemyScript.gridMax * m_gridSize.x) {
			enemyTransform.vel = sf::Vector2f(-50, 0);

			auto& enemyAnimation = enemy->getComponent<CAnimation>().animation;
			enemyAnimation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyLeft")).animation;
			enemyAnimation.play();
		}
		else if (currentPos.x <= enemyScript.gridMin * m_gridSize.x) {
			enemyTransform.vel = sf::Vector2f(50, 0);

			auto& enemyAnimation = enemy->getComponent<CAnimation>().animation;
			enemyAnimation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyRight")).animation;
			enemyAnimation.play();
		}
	}
}

void Scene_Berzerk::sRender() {
	m_game->window().setView(m_worldView);

	// Draw black background
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(m_game->window().getSize().x, m_game->window().getSize().y));
	background.setFillColor(sf::Color::Black);
	m_game->window().draw(background);

	// draw bkg first
	for (auto& e : m_entityManager.getEntities("bkg")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	sf::View view = m_game->window().getView();

	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CSprite>())
		{
			auto& sprite = e->getComponent<CSprite>().sprite;
			if (e->hasComponent<CTransform>()) {
				auto& tfm = e->getComponent<CTransform>();
				sprite.setPosition(tfm.pos);

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

		else if (action.name() == "CARRY_SPEAR") { m_player->getComponent<CPowerUps>().dragonSpearCarried = true; }
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
		else if (action.name() == "RELEASE_SPEAR") { m_player->getComponent<CPowerUps>().dragonSpearCarried = false; }
	}
}


void Scene_Berzerk::spawnPlayer(sf::Vector2f pos) {

	m_player = m_entityManager.addEntity("player");

	sf::Vector2f boundingBoxOffset{ 0.f, 10.f }; // Moving the bounding box down a bit
	m_player->setBoundingBoxOffset(boundingBoxOffset);

	m_player->addComponent<CTransform>(pos);
	m_player->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	m_player->addComponent<CInput>();

	if (m_player->getComponent<CPowerUps>().dragonSpearCarried) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("spearRight"));
	}
	else {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
	}
}

// spawn Enemies
void Scene_Berzerk::spawnEnemy(const sf::Vector2f& initialPosition, const sf::Vector2f& initialVelocity, float gridMin, float gridMax) {
	auto enemy2 = m_entityManager.addEntity("enemy");

	enemy2->addComponent<CTransform>(initialPosition, initialVelocity);
	enemy2->addComponent<CBoundingBox>(sf::Vector2f(80.f, 40.f));

	if (initialVelocity.x < 0) {
		enemy2->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyLeft")).animation.play();
	}
	else {
		enemy2->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyRight")).animation.play();
	}

	enemy2->addComponent<CScript>(gridMin, gridMax);
}

// spawn Dragon Spear
void Scene_Berzerk::spawnDragonSpear(const sf::Vector2f& initialPosition) {
	auto dragonSpear = m_entityManager.addEntity("dragonSpear");

	dragonSpear->addComponent<CTransform>(initialPosition);
	dragonSpear->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	dragonSpear->addComponent<CAnimation>(Assets::getInstance().getAnimation("dragonSpear"));
}

void Scene_Berzerk::spawnPowerUp(const sf::Vector2f& initialPosition) {
	auto powerUp = m_entityManager.addEntity("powerUp");
	powerUp->addComponent<CTransform>(initialPosition);
	powerUp->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	powerUp->addComponent<CAnimation>(Assets::getInstance().getAnimation("powerUp"));
}

sf::FloatRect Scene_Berzerk::getViewBounds() {
	return sf::FloatRect();
}

void Scene_Berzerk::sCollisions() {
	adjustPlayerPosition();

	if (m_player->getComponent<CState>().state == "dead") {
		return;
	}

	// player with tile
	auto& players = m_entityManager.getEntities("player");
	auto& tiles = m_entityManager.getEntities("Tile");

	for (auto p : players) {
		for (auto& t : tiles) {

			auto overlap = Physics::getOverlap(p, t);
			if (overlap.x > 0 && overlap.y > 0)
			{
				auto prevOverlap = Physics::getOverlap(p, t);

				auto& ptx = p->getComponent<CTransform>();
				auto& ttx = t->getComponent<CTransform>();

				// Collision with the y axis
				if (prevOverlap.x > prevOverlap.y) {
					if (ptx.prevPos.y < ttx.prevPos.y) {
						p->getComponent<CTransform>().pos.y -= overlap.y;
					}
					else if (ptx.prevPos.y > ttx.prevPos.y) {
						p->getComponent<CTransform>().pos.y += overlap.y;
					}

				}
				// Collision with the x axis
				else {
					if (ptx.prevPos.x < ttx.prevPos.x) {
						p->getComponent<CTransform>().pos.x -= overlap.x;
					}
					else {
						p->getComponent<CTransform>().pos.x += overlap.x;
					}
				}
			}
		}

		// Player vs Enemies

		auto& enemies = m_entityManager.getEntities("enemy");

		for (auto& enemy : enemies) {
			if (checkCollision(*m_player, *enemy)) {
				onPlayerCollision();
				return;
			}
		}

		// Player vs Dragon Spear
		auto& dragonSpears = m_entityManager.getEntities("dragonSpear");

		sf::Vector2f collisionDirection;
		for (auto& spear : dragonSpears) {
			if (checkCollision(*m_player, *spear)) {
				// Remove the dragon spear from the scene
				spear->destroy();

				auto& playerTransform = m_player->getComponent<CTransform>();
				auto& spearTransform = spear->getComponent<CTransform>();

				sf::Vector2f collisionDirection = spearTransform.pos - playerTransform.pos;
				collisionDirection = normalize(collisionDirection);

				// Set the player state to 'dragonSpearCarried'
				m_player->getComponent<CPowerUps>().dragonSpearCarried = true;

				playerMovement();

				return;
			}
		}

		// Player vs Power Up
		auto& powerUps = m_entityManager.getEntities("powerUp");
		for (auto& powerUp : powerUps) {
			if (checkCollision(*m_player, *powerUp)) {
				powerUp->destroy();

				// speed up the player 1.1 times
				// get CTransform of player and multiply speed by 1.1
				auto& playerTransform = m_player->getComponent<CTransform>();
				// Continuar...


				return;
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
	// see the animation
	m_player->getComponent<CAnimation>().animation.play();

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
		//respawnPlayer();
		m_player->addComponent<CState>().state = "dead";
	}
}


void Scene_Berzerk::respawnPlayer() {

	// Player Initial Position
	sf::Vector2f initialPlayerPosition;
	initialPlayerPosition.x = 0.f - 20.f;
	initialPlayerPosition.y = m_game->window().getSize().y - (600.f - 260.f);

	// Setting the player position to the initial position
	m_player->getComponent<CTransform>().pos = initialPlayerPosition;

	/*auto& playerState = m_player->getComponent<CState>().state;
	playerState = "grounded";*/

	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
}

void Scene_Berzerk::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();
	m_entityManager.update();

	if (m_isPaused)
		return;

	updateScoreText(); // update score text
	updateLivesText();

	/// consertar isto

	//updateEnemyChase();
	if (m_player->getComponent<CState>().state != "dead") {
		updateEnemyChase();
	}
	// frose the enemy when the player is dead
	else
	{
		auto& enemies = m_entityManager.getEntities("enemy");
		for (auto& enemy : enemies) {
			auto& enemyAnimation = enemy->getComponent<CAnimation>().animation;
			// velocity of the enemy is 0
			enemy->getComponent<CTransform>().vel = sf::Vector2f(0.f, 0.f);
			enemyAnimation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyDown")).animation;
			//enemyAnimation.stop();
		}
	}

	sAnimation(dt);
	sMovement(dt);
	adjustPlayerPosition();
	checkPlayerState();
	sCollisions();
}

void Scene_Berzerk::updateEnemyChase() {
	auto playerPos = m_player->getComponent<CTransform>().pos;
	auto& enemies = m_entityManager.getEntities("enemy");

	for (auto& enemy : enemies) {
		auto& enemyTransform = enemy->getComponent<CTransform>();
		auto& enemyAnimation = enemy->getComponent<CAnimation>().animation;

		sf::Vector2f currentVelocity = enemyTransform.vel;

		float enemyChaseSpeed = sqrt(currentVelocity.x * currentVelocity.x + currentVelocity.y * currentVelocity.y) * 1.005f;

		if (playerPos.y >= enemyTransform.pos.y - m_gridSize.y / 2 &&
			playerPos.y <= enemyTransform.pos.y + m_gridSize.y / 2) {

			sf::Vector2f direction = playerPos - enemyTransform.pos;
			direction = normalize(direction);

			enemyTransform.vel = direction * enemyChaseSpeed;

			// Update enemy animation based on velocity direction
			if (direction.x > 0) {
				enemyAnimation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyRight")).animation;
			}
			else {
				enemyAnimation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("enemyLeft")).animation;
			}

			enemyAnimation.play();
		}
	}
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

	auto& list = m_entityManager.getEntities();
	for (auto& e : m_entityManager.getEntities()) {

		// update all animations
		if (e->hasComponent<CAnimation>()) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);
		}
	}
}


void Scene_Berzerk::adjustPlayerPosition() {
	auto& center = m_worldView.getCenter();
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

			e->addComponent<CBoundingBox>(sf::Vector2f(rect.width, rect.height));
			sprite.setTextureRect(rect);

			pos.x += (rect.width / 2.0f);
			pos.y += (rect.height / 2.0f);

			auto& tfm = e->addComponent<CTransform>(pos);

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


