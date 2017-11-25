#pragma once

#include <Core/Engine.h>
#include <RacingGame/SpatialObject.h>

class Buff;

class Car : public SpatialObject
{
public:
	Car();
	virtual ~Car() {};

	virtual void Update(float deltaTimeSeconds) override;

	float GetVelocity();
	void SetVelocity(float v);

	float GetMaxSpeed();
	void SetMaxSpeed(float speed);

	float GetAcceleration();
	void SetAcceleration(float a);

	glm::vec3 GetAngularVelocity();
	void SetAngularVelocity(const glm::vec3 &val);

	glm::vec3 GetDirection();
	void SetDirection(const glm::vec3 &val);

	void OnCollision(SpatialObject *o);
	void OnCollision(Buff *b);

protected:
	float acceleration;
	float velocity;
	float maxSpeed;
	glm::vec3 angularVelocity;
	glm::vec3 direction;

public:
	int lives, score;
};
