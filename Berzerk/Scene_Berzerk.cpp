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
#include "Scene_Credits.h"
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
	// I could make it in level.txt, but I have simplified for this project.
	auto pos = m_worldView.getSize();
	pos.x = 0.f - 20.f;
	pos.y -= 600.f - 260.f;

	spawnPlayer(pos);

	MusicPlayer::getInstance().play("gameTheme");
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

	// space to attack
	registerAction(sf::Keyboard::Space, "ATTACK");
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
	auto& state = m_player->getComponent<CState>();

	// Define player speed
	float playerSpeed = m_config.playerSpeed;

	// Diagonal movements
	if (pInput.up && pInput.left) {
		pv.x -= 1;
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.up && pInput.right) {
		pv.x += 1;
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down && pInput.left) {
		pv.x -= 1;
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down && pInput.right) {
		pv.x += 1;
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}

	// Single movements
	else if (pInput.left) {
		pv.x -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearLeft" : "left";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.right) {
		pv.x += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearRight" : "right";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.up) {
		pv.y -= 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearUp" : "up";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.down) {
		pv.y += 1;

		std::string animationName = pPowerUps.dragonSpearCarried ? "spearDown" : "down";

		// If spearLeft and pInput.attack is true, then play the attack animation
		if (pPowerUps.dragonSpearCarried && pInput.attack) {
			animationName = playAttackAnimation();
		}

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pInput.attack && pPowerUps.dragonSpearCarried)
	{
		std::string animationName = playAttackAnimation();

		if (animation.getName() != animationName)
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
	}
	else if (pPowerUps.dragonSpearCarried && !pInput.attack)
	{
		m_player->getComponent<CBoundingBox>().size = sf::Vector2f(25.0f, 25.0f);
		m_player->getComponent<CAnimation>().animation.stop();
	}
	else
	{
		m_player->getComponent<CAnimation>().animation.stop();
	}

	pv = normalize(pv);
	m_player->getComponent<CTransform>().vel = m_config.playerSpeed * pv * m_player->getComponent<CPowerUps>().velocity;
}

std::string Scene_Berzerk::playAttackAnimation()
{
 	auto& animation = m_player->getComponent<CAnimation>().animation;
	std::string oldAnimation = animation.getName();
	if (!oldAnimation.contains("attack"))
	{
		oldAnimation.erase(0, 5);
		std::string newAnimation = "attack" + oldAnimation;

		sf::Vector2f newSize;
		if (oldAnimation == "Down" || oldAnimation == "Up") {
			newSize = sf::Vector2f(25.f, 80.f);
		}
		else if (oldAnimation == "Left" || oldAnimation == "Right") {
			newSize = sf::Vector2f(80.f, 25.f);
		}

		// Update the size of the player bounding box
		m_player->getComponent<CBoundingBox>().size = newSize;

		// Set attack states
		if (oldAnimation == "Down") {
			m_player->getComponent<CState>().state = "attackDown";
		}
		else if (oldAnimation == "Up") {
			m_player->getComponent<CState>().state = "attackUp";
		}
		else if (oldAnimation == "Left") {
			m_player->getComponent<CState>().state = "attackLeft";
		}
		else if (oldAnimation == "Right") {
			m_player->getComponent<CState>().state = "attackRight";
		}

		// play sound effect of attack
		SoundPlayer::getInstance().play("attack", m_player->getComponent<CTransform>().pos);
			
		return newAnimation;
	}

	return oldAnimation;
}

void Scene_Berzerk::enemyMovement() {

	auto& enemies = m_entityManager.getEntities("enemy");
	auto& playerPos = m_player->getComponent<CTransform>().pos;

	for (auto& enemy : enemies)
	{
		auto& enemyTransform = enemy->getComponent<CTransform>();
		auto& enemyScript = enemy->getComponent<CScript>();
		sf::Vector2f currentPos = enemyTransform.pos;

		if (enemy->getComponent<CState>().state == "patrol")
		{
			if (enemyTransform.pos.y > enemyScript.patrolY)
			{
				if (enemyTransform.prevPos.y < enemyScript.patrolY)
				{
					enemyTransform.pos.y = enemyScript.patrolY;

					if (enemyScript.oldVelX > 0) enemyScript.oldVelX = 50.0;
					else if (enemyScript.oldVelX < 0) enemyScript.oldVelX = -50.0;

					enemyTransform.vel = sf::Vector2f(enemyScript.oldVelX, 0);
				}
				else
				{
					if (enemyTransform.vel.x != 0) enemyScript.oldVelX = enemyTransform.vel.x;
					enemyTransform.vel = sf::Vector2f(0, -20);
				}
			}
			else if (enemyTransform.pos.y < enemyScript.patrolY)
			{
				if (enemyTransform.prevPos.y > enemyScript.patrolY)
				{
					enemyTransform.pos.y = enemyScript.patrolY;

					if (enemyScript.oldVelX > 0) enemyScript.oldVelX = 50.0;
					else if (enemyScript.oldVelX < 0) enemyScript.oldVelX = -50.0;

					enemyTransform.vel = sf::Vector2f(enemyScript.oldVelX, 0);
				}
				else
				{
					if (enemyTransform.vel.x != 0) enemyScript.oldVelX = enemyTransform.vel.x;
					enemyTransform.vel = sf::Vector2f(0, 20);
				}
			}

			if (enemyTransform.pos.x >= enemyScript.gridMax * m_gridSize.x)
			{
				enemyTransform.vel = sf::Vector2f(-50, 0);
			}
			else if (enemyTransform.pos.x <= enemyScript.gridMin * m_gridSize.x)
			{
				enemyTransform.vel = sf::Vector2f(50, 0);
			}
		}
		else if (enemy->getComponent<CState>().state == "chase")
		{
			auto& playerPos = m_player->getComponent<CTransform>().pos;
			sf::Vector2f currentVelocity = enemyTransform.vel;

			float chaseSpeed = 110.0f;

			sf::Vector2f direction = playerPos - enemyTransform.pos;
			direction = normalize(direction);

			enemyTransform.vel = direction * chaseSpeed;
		}

		auto& animation = enemy->getComponent<CAnimation>().animation;
		std::string str = enemy->getComponent<CString>().str;
		std::string animationName;

		if (enemyTransform.vel.x > 0) {
			animationName = str + "Right";
		}
		else if (enemyTransform.vel.x < 0){
			animationName = str + "Left";
		}
		else if (enemyTransform.vel.y > 0) {
			animationName = str + "Down";
		}
		else if (enemyTransform.vel.y < 0) {
			animationName = str + "Up";
		}

		if (animation.getName() != animationName)
			animation = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation(animationName)).animation;
		animation.play();
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
		else if (action.name() == "ATTACK") { m_player->getComponent<CInput>().attack = true; }

		else if (action.name() == "CARRY_SPEAR") { m_player->getComponent<CPowerUps>().dragonSpearCarried = true; }
	}

	// Key release
	else if (action.type() == "END") {
		// Player control
		if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") {	m_player->getComponent<CInput>().down = false; }
		else if (action.name() == "ATTACK") { m_player->getComponent<CInput>().attack = false; }
		else if (action.name() == "RELEASE_SPEAR") { m_player->getComponent<CPowerUps>().dragonSpearCarried = false; }
	}
}

void Scene_Berzerk::spawnPlayer(sf::Vector2f pos) {

	m_player = m_entityManager.addEntity("player");

	sf::Vector2f boundingBoxOffset{ 0.f, 10.f }; // Moving down the bounding box a bit
	m_player->setBoundingBoxOffset(boundingBoxOffset);

	m_player->addComponent<CTransform>(pos);
	m_player->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	m_player->addComponent<CInput>();

	m_player->addComponent<CState>().state = "alive";

	if (m_player->getComponent<CPowerUps>().dragonSpearCarried) {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("spearRight"));
	}
	else {
		m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));
	}
}

void Scene_Berzerk::spawnEnemy(
	const sf::Vector2f& initialPosition,
	const sf::Vector2f& initialVelocity,
	float gridMin,
	float gridMax,
	const std::string& name)
{
	auto enemy = m_entityManager.addEntity("enemy");

	enemy->addComponent<CTransform>(initialPosition, initialVelocity);
	enemy->addComponent<CBoundingBox>(sf::Vector2f(80.f, 40.f));

	if (initialVelocity.x < 0) {
		enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation(name + "Left")).animation.play();
	}
	else {
		enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation(name + "Right")).animation.play();
	}

	enemy->addComponent<CState>("patrol");
	enemy->addComponent<CScript>(gridMin, gridMax, initialPosition.y);
	enemy->addComponent<CString>(name);
}

void Scene_Berzerk::spawnDragonSpear(const sf::Vector2f& initialPosition) {
	auto dragonSpear = m_entityManager.addEntity("dragonSpear");

	dragonSpear->addComponent<CTransform>(initialPosition);
	dragonSpear->addComponent<CBoundingBox>(sf::Vector2f(50.f, 50.f));
	dragonSpear->addComponent<CAnimation>(Assets::getInstance().getAnimation("dragonSpear"));
}

void Scene_Berzerk::spawnPowerUp(const sf::Vector2f& initialPosition) {
	auto powerUp = m_entityManager.addEntity("powerUp");
	powerUp->addComponent<CTransform>(initialPosition);
	powerUp->addComponent<CBoundingBox>(sf::Vector2f(25.f, 25.f));
	powerUp->addComponent<CAnimation>(Assets::getInstance().getAnimation("powerUp")).animation.play();
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

			auto& playerPos = p->getComponent<CTransform>().pos;
			auto& enemyTransform = enemy->getComponent<CTransform>().pos;

			if ((playerPos.y >= enemyTransform.y - m_gridSize.y && playerPos.y <= enemyTransform.y + m_gridSize.y) ||
				(playerPos.x <= enemyTransform.x - m_gridSize.x * 2 && playerPos.x >= enemyTransform.x + m_gridSize.x * 2))
			{
				enemy->getComponent<CState>().state = "chase";
			}
			else
			{
				enemy->getComponent<CState>().state = "patrol";
			}

			if (checkCollision(*m_player, *enemy)) {
				if (m_player->getComponent<CInput>().attack) {

					SoundPlayer::getInstance().play("deathEnemy", m_player->getComponent<CTransform>().pos);
					enemy->destroy();
				}
				else {
					onPlayerCollision();
				}
			}
		}

		// Player vs Dragon Spear
		auto& dragonSpears = m_entityManager.getEntities("dragonSpear");

		sf::Vector2f collisionDirection;
		for (auto& spear : dragonSpears) {
			if (checkCollision(*m_player, *spear)) {

				// Play the sound effect
				SoundPlayer::getInstance().play("pickUpSpear", m_player->getComponent<CTransform>().pos);

				// Remove the dragon spear from the scene
				spear->destroy();

				auto& playerTransform = m_player->getComponent<CTransform>();
				auto& spearTransform = spear->getComponent<CTransform>();

				sf::Vector2f collisionDirection = spearTransform.pos - playerTransform.pos;
				collisionDirection = normalize(collisionDirection);

				// Set the player state to 'dragonSpearCarried'
				m_player->getComponent<CPowerUps>().dragonSpearCarried = true;

				playerMovement();
			}
		}

		// Player vs Power Up
		auto& powerUps = m_entityManager.getEntities("powerUp");
		for (auto& powerUp : powerUps) {
			if (checkCollision(*m_player, *powerUp)) {
				SoundPlayer::getInstance().play("powerUp", m_player->getComponent<CTransform>().pos);
				powerUp->destroy();

				auto& playerPowerUps = m_player->getComponent<CPowerUps>();
				playerPowerUps.velocity++;
				playerPowerUps.powerUpCollected = true;
			}
		}

		// Player vs Exit
		for (auto& exit : m_entityManager.getEntities("Exit"))
		{
			auto overlap = Physics::getOverlap(p, exit);
			if (overlap.x > 0 && overlap.y > 0)
			{
				auto& str = exit->getComponent<CString>().str;

				auto& playerPowerUps = p->getComponent<CPowerUps>();
				// Condition to allow changing scene if the player carries power ups
				if (playerPowerUps.dragonSpearCarried && playerPowerUps.powerUpCollected) 
				{
					if (str == "quit")
					{
						m_game->changeScene("CREDITS", std::make_shared<Scene_Credits>(m_game));
					}
					else
					{
						m_game->changeScene("PLAY", std::make_shared<Scene_Berzerk>(m_game, str), true);
					}
				}
			}
		}
	}

	// Enemies vs Tiles
	auto& enemies = m_entityManager.getEntities("enemy");

	for (auto& enemy : enemies)
	{
		for (auto& t : tiles) {

			auto overlap = Physics::getOverlap(enemy, t);
			if (overlap.x > 0 && overlap.y > 0)
			{
				auto prevOverlap = Physics::getOverlap(enemy, t);

				auto& ptx = enemy->getComponent<CTransform>();
				auto& ttx = t->getComponent<CTransform>();

				// Collision with the y axis
				if (prevOverlap.x > prevOverlap.y) {
					if (ptx.prevPos.y < ttx.prevPos.y) {
						enemy->getComponent<CTransform>().pos.y -= overlap.y;
					}
					else if (ptx.prevPos.y > ttx.prevPos.y) {
						enemy->getComponent<CTransform>().pos.y += overlap.y;
					}
				}
				// Collision with the x axis
				else {
					if (ptx.prevPos.x < ttx.prevPos.x) {
						enemy->getComponent<CTransform>().pos.x -= overlap.x;
					}
					else {
						enemy->getComponent<CTransform>().pos.x += overlap.x;
					}
				}
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
	m_player->getComponent<CAnimation>().animation.play();

	// Deccrement life
	Assets::getInstance().decrementLife(1);

	for (auto& e : m_entityManager.getEntities("enemy"))
	{
		e->getComponent<CAnimation>().animation.stop();
	}

	m_player->addComponent<CState>().state = "dead";
}

void Scene_Berzerk::respawnPlayer() {

	// Player Initial Position
	sf::Vector2f initialPlayerPosition;
	initialPlayerPosition.x = 0.f - 20.f;
	initialPlayerPosition.y = m_game->window().getSize().y - (600.f - 260.f);

	// Setting the player position to the initial position
	m_player->getComponent<CTransform>().pos = initialPlayerPosition;
	m_player->getComponent<CTransform>().vel = { 0, 0 };

	m_player->addComponent<CState>();
	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right"));

	m_player->addComponent<CState>().state = "alive";
}

void Scene_Berzerk::sUpdate(sf::Time dt) {

	SoundPlayer::getInstance().removeStoppedSounds();
	m_entityManager.update();

	if (m_isPaused) 
		return;

	sAnimation(dt);

	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead") {

		if (m_player->getComponent<CAnimation>().animation.hasEnded()) {
			if (Assets::getInstance().getLife() <= 0)
				m_game->quitLevel();

			respawnPlayer();
			for (auto& e : m_entityManager.getEntities("enemy")) {
				e->getComponent<CAnimation>().animation.play();
			}			
		}
		return;
	}

	sMovement(dt);
	adjustPlayerPosition();
	checkPlayerState();
	sCollisions();
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
		else if (token == "Exit") {
			sf::Vector2f pos;
			sf::Vector2f size;
			std::string nextLevel;
			config >> pos.x >> pos.y >> size.x >> size.y >> nextLevel;

			auto e = m_entityManager.addEntity("Exit");

			pos.x += (size.x / 2.0f);
			pos.y += (size.y / 2.0f);

			e->addComponent<CTransform>(pos);
			e->addComponent<CBoundingBox>(size);
			e->addComponent<CString>(nextLevel);
		}
		else if (token == "Enemy")
		{
			sf::Vector2f pos;
			sf::Vector2f vel;
			float min, max;
			std::string name;
			config >> pos.x >> pos.y >> vel.x >> vel.y >> min >> max >> name;

			// Set the center of the grid cell
			float xCenter = pos.x * m_gridSize.x + m_gridSize.x / 2.0f;
			float yCenter = m_game->window().getSize().y - (pos.y * m_gridSize.y + m_gridSize.y / 2.0f);

			// Spawn the enemy
			spawnEnemy(sf::Vector2f(xCenter, yCenter), vel, min, max, name);
		}
		else if (token == "Spear")
		{
			sf::Vector2f pos;
			config >> pos.x >> pos.y;

			// spawn dragonSpare
			spawnDragonSpear(sf::Vector2f(pos.x * m_gridSize.x, (600 - pos.y * m_gridSize.y) - m_gridSize.y / 2));
		}
		else if (token == "PowerUp")
		{
			sf::Vector2f pos;
			config >> pos.x >> pos.y;

			// spawn power up (position 18, 2)
			spawnPowerUp(sf::Vector2f(18 * m_gridSize.x, (600 - pos.y * m_gridSize.y) - m_gridSize.y / 2));
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


