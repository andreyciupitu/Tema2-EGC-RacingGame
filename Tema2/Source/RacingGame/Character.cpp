#include "Character.h"

#include <RacingGame/Geometry3D.h>
#include <RacingGame/SpatialObject.h>

Character::Character()
{
	maxSpeed = 0.0f;
	angularVelocity = 0.0f;
	aligning = false;
	moving = false;
	angularVelocity = 0.0f;
	velocity = glm::vec3(0, 0, 0);
	targetOrientation = 0.0f;
}

void Character::Update(float deltaTimeSeconds)
{
	// Update orientation and position
	rotation.y += angularVelocity * deltaTimeSeconds;
	transform += velocity * deltaTimeSeconds;

	// Keep the orientation in (-PI, PI)
	if (rotation.y > M_PI)
		rotation.y -= 2 * M_PI;
	if (rotation.y < -1 * M_PI)
		rotation.y += 2 * M_PI;
}

void Character::Seek(const glm::vec3 &target)
{
	glm::vec3 direction = target - transform;
	Align(direction);
	float distance = glm::distance(target, transform);
	velocity = direction / distance * maxSpeed;
	moving = true;
}

void Character::Align(const glm::vec3 &direction)
{
	rotation.y = atan2(direction.y, direction.x);
}

bool Character::IsMoving()
{
	return moving;
}

glm::vec3 Character::GetVelocity()
{
	return velocity;
}

void Character::SetVelocity(glm::vec3 v)
{
	velocity = v;
}

float Character::GetMaxSpeed()
{
	return maxSpeed;
}

void Character::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
}

float Character::GetMaxAnglarSpeed()
{
	return maxAngularSpeed;
}

void Character::SetMaxAngularSpeed(float speed)
{
	maxAngularSpeed = speed;
}

void Character::OnCollision(SpatialObject *o, glm::vec3 collisionNormal)
{
	velocity = glm::vec3(0);
	moving = false;
	aligning = false;
}