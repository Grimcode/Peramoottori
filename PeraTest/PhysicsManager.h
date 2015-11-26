#pragma once
#include <vector>
#include "Physics.h"
#include <scene\GameEntity.h>
#include <scene\Transformable.h>
#include <graphics\Rectangle.h>

class PhysicsManager
{
public:
	PhysicsManager(){ physicsUpdateRate = 0.1; physicsTime = 0; };
	PhysicsManager(glm::vec2 worldLimits, float physicUpdateRate);
	~PhysicsManager();
	
	void AddPhysics(pm::GameEntity* target);
	void RemoveGameEntity();
	void Update(float time);

	void UpdatePhysics(Physics* physics);

	// Peri ja anna k�ytt�j�n tehd� n�m� itse.
	glm::vec2 SetPosition(Physics* physics);

	void KeepInsideScreen(Physics* physics);

	glm::vec2 CheckLimits(glm::vec2 position);

private:
	std::vector<Physics*> physicsVector;
	float physicsUpdateRate;
	float physicsTime;
	// Peri ja anna k�ytt�j�n tehd� n�m� itse.
	glm::vec2 worldLimits;
};
