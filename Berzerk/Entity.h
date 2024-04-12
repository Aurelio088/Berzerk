//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_ENTITY_H
#define BREAKOUT_ENTITY_H

#include <tuple>
#include <string>
#include "Components.h"

// forward declarations
class EntityManager;

using ComponentTuple = std::tuple<CSprite, CAnimation, CState, CTransform, CBoundingBox, CInput, CPowerUps, CScript, CString>;

class Entity {
private:
	friend class EntityManager;
	Entity(size_t id, const std::string& tag);      // private ctor, entities can only be created by EntityManager

	const size_t            m_id{ 0 };
	const std::string       m_tag{ "Default" };
	bool                    m_active{ true };
	ComponentTuple          m_components;

	sf::Vector2f m_boundingBoxOffset{ 0.f, 0.f };	// Add by Aurelio Rodrigues

public:

	void                    destroy();
	const size_t& getId() const;
	const std::string& getTag() const;
	bool                    isActive() const;

	// Component API
	template<typename T>
	inline bool hasComponent() const {
		return getComponent<T>().has;
	}

	template<typename T, typename... TArgs>
	inline T& addComponent(TArgs &&... mArgs) {
		auto& component = getComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);

		// Add by Aurelio
		if constexpr (std::is_same<T, CBoundingBox>::value) {
			component.offset = m_boundingBoxOffset;
		}
		// End add by Aurelio

		component.has = true;
		return component;
	}

	// Add by Aurelio
	inline void setBoundingBoxOffset(const sf::Vector2f& offset) {
		m_boundingBoxOffset = offset;
	}
	// End add by Aurelio

	template<typename T>
	inline bool removeComponent() { // Removed "const"
		return getComponent<T>().has = false;
	}

	// I have removed the const from the return type of getComponent
	// However, I just noticed that going to finish the assignment
	// That's why I have not implemented removeComponent in other entities

	template<typename T>
	inline T& getComponent() {
		return std::get<T>(m_components);
	}

	template<typename T>
	inline const T& getComponent() const {
		return std::get<T>(m_components);
	}
};

#endif //BREAKOUT_ENTITY_H
