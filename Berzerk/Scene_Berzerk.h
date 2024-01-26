//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_SCENE_BREAKOUT_H
#define BREAKOUT_SCENE_BREAKOUT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"
#include <unordered_map>



class Scene_Berzerk : public Scene {
private:
	sPtrEntt        m_player{ nullptr };
	sf::View        m_worldView;
	sf::FloatRect   m_worldBounds;

	sf::Text		m_scoreText;								// Added by Aurelio Rodrigues
	sf::Text		m_livesText;								// Added by Aurelio Rodrigues
	sf::Text		m_endText;									// Added by Aurelio Rodrigues

	sf::Font        m_font;										// Added by Aurelio Rodrigues

	sf::Clock		gapTimer;									// Added by Aurelio Rodrigues
	sf::Clock		bugIconTimer;								// Added by Aurelio Rodrigues
	sf::Time		bugIconDuration = sf::seconds(5.0f);		// Added by Aurelio Rodrigues

	bool			m_drawTextures{ true };
	bool			m_drawAABB{ false };
	bool			m_drawGrid{ false };

	int             m_score{ 0 };
	int				m_lillyPadsOccupied = 0;


	//systems
	void            sMovement(sf::Time dt);
	void            sCollisions();
	void            sUpdate(sf::Time dt);
	void            sAnimation(sf::Time dt);

	void	        onEnd() override;

	std::unordered_map<std::string, bool> m_laneCrossed;

	// helper functions
	void            playerMovement();
	void            adjustPlayerPosition();
	void            checkPlayerState();
	void	        registerActions();
	void            spawnPlayer(sf::Vector2f pos);

	void            spawnLane1();								// Added by Aurelio Rodrigues
	void            spawnLane2(); 								// Added by Aurelio Rodrigues
	void            spawnLane3(); 								// Added by Aurelio Rodrigues
	void            spawnLane4(); 								// Added by Aurelio Rodrigues
	void            spawnLane5(); 								// Added by Aurelio Rodrigues
	void            spawnLane6(); 								// Added by Aurelio Rodrigues
	void            spawnLane7(); 								// Added by Aurelio Rodrigues
	void            spawnLane8(); 								// Added by Aurelio Rodrigues
	void            spawnLane9(); 								// Added by Aurelio Rodrigues
	void            spawnLane10(); 								// Added by Aurelio Rodrigues
	void            spawnLane11(); 								// Added by Aurelio Rodrigues
	void            spawnBugIcon(); 							// Added by Aurelio Rodrigues

	// Adicionado por mim
	void            updateScoreText(); 							// Added by Aurelio Rodrigues
	void            updateLivesText(); 							// Added by Aurelio Rodrigues
	void            winningMessage(); 							// Added by Aurelio Rodrigues

	void 		    handleCarCollision(std::shared_ptr<Entity> e);
	void 		    handleTractorCollision(std::shared_ptr<Entity> e);

	void 		    handleRaceCarCollision(std::shared_ptr<Entity> e);

	void 		    respawnPlayer();
	void 		    onPlayerCollision(); 						// Added by Aurelio Rodrigues
	void		    onPlayerCollisionDrive(); 					// Added by Aurelio Rodrigues
	void		    onPlayerCollisionLillyPad(); 				// Added by Aurelio Rodrigues
	bool 		    checkCollision(Entity& entity1, Entity& entity2); // Added by Aurelio Rodrigues


	// FIM Adicionado por mim

	void            init(const std::string& path);
	void            loadLevel(const std::string& path);
	sf::FloatRect   getViewBounds();

public:

	Scene_Berzerk(GameEngine* gameEngine, const std::string& levelPath);

	void		  update(sf::Time dt) override;
	void		  sDoAction(const Command& command) override;
	void		  sRender() override;


};



#endif //BREAKOUT_SCENE_BREAKOUT_H
