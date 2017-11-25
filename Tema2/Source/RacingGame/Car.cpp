#include "Car.h"

#include <RacingGame/Geometry3D.h>
#include <RacingGame/Buff.h>
#include <RacingGame/SpatialObject.h>

Car::Car()
{
	maxSpeed = 0.0f;
	angularVelocity = glm::vec3(0);
	acceleration = 0.0f;
	velocity = 0;
	direction = glm::vec3(1, 0, 0);
	lives = 3;
	score = 0;
}

void Car::Update(float deltaTimeSeconds)
{
	// Update rotation and position
	float w = angularVelocity.y * deltaTimeSeconds;
	rotation += angularVelocity * deltaTimeSeconds;
	direction = glm::vec3(Transform::RotateOY(w) * glm::vec4(direction, 1));
	direction = glm::normalize(direction);
	transform += velocity * deltaTimeSeconds * direction;

	// Update velocity
	velocity += acceleration * deltaTimeSeconds;

	// Cap velocity
	if (velocity > maxSpeed)
		velocity = maxSpeed;
	if (velocity < 0)
		velocity = 0;
}

float Car::GetVelocity()
{
	return velocity;
}

void Car::SetVelocity(float v)
{
	velocity = v;
}

float Car::GetMaxSpeed()
{
	return maxSpeed;
}

void Car::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
}

float Car::GetAcceleration()
{
	return acceleration;
}

void Car::SetAcceleration(float a)
{
	acceleration = a;
}

glm::vec3 Car::GetAngularVelocity() 
{
	return angularVelocity;
}

void Car::SetAngularVelocity(const glm::vec3 &val)
{
	angularVelocity = val;
}

glm::vec3 Car::GetDirection()
{
	return direction;
}

void Car::SetDirection(const glm::vec3 &val)
{
	direction = val;
}

void Car::OnCollision(SpatialObject *o)
{
	velocity = 0;
	lives--;
	score -= 100;
}

void Car::OnCollision(Buff *b)
{
	switch (b->type)
	{
	case Buff::BuffType::SLOW:
		velocity *= 0.7f;
		score -= 50;
		break;
	case Buff::BuffType::SPEEDUP:
		velocity += maxSpeed / 4;
		velocity = std::max(velocity, maxSpeed);
		score += 50;
		break;
	case Buff::BuffType::LIFE:
		lives++;
		score += 100;
		break;
	}
}