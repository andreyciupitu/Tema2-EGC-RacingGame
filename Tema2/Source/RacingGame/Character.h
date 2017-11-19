#pragma once

#include <Core/Engine.h>
#include <RacingGame/SpatialObject.h>

class Character : public SpatialObject
{
public:
	Character();
	virtual ~Character() {};

	virtual void Update(float deltaTimeSeconds) override;

	// Moves the character towards the position of the target
	void Seek(const glm::vec3 &target);

	// Aligns the character with the given direction
	void Align(const glm::vec3 &direction);

	bool IsMoving();

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 v);

	float GetMaxSpeed();
	void SetMaxSpeed(float speed);

	float GetMaxAnglarSpeed();
	void SetMaxAngularSpeed(float speed);

	// Collision handling functions
	virtual void OnCollision(SpatialObject *o, glm::vec3 collisionNormal);

protected:
	bool moving, aligning;
	glm::vec3 velocity;
	float maxSpeed;
	float angularVelocity;
	float maxAngularSpeed;
	float targetOrientation;
};
