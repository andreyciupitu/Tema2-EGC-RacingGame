#pragma once

#include <RacingGame/SpatialObject.h>
#include <include/glm.h>

class Buff : public SpatialObject
{
public:
	enum BuffType
	{
		SLOW = 0, SPEEDUP = 1, LIFE = 2 
	};

	Buff(BuffType type) 
	{
		collected = false;
		this->type = type;
		baseColors[0] = glm::vec3(0.364, 0.917, 0.164);
		baseColors[1] = glm::vec3(0.917, 0.913, 0.164);
		baseColors[2] = glm::vec3(1, 0.141, 0.615);
		direction = glm::vec3(1, 0, 0);
		maxSpeed = 0.0f;
		target = glm::vec3(0);
	}

	bool IsCollected() { return collected; }
	void Spawn() { collected = false; }
	void Despawn() { collected = true; }

	void Seek(const glm::vec3& target)
	{
		if (glm::dot(target - transform, direction) < 0)
		{
			velocity = 0;
			return;
		}
		direction = target - transform;
		float distance = glm::distance(target, transform);
		velocity = maxSpeed / distance;
	}

	virtual void Update(float deltaTimeSeconds) override
	{/*
		if (glm::dot(target - transform, direction) < 0)
			velocity = 0;*/
		if (maxSpeed == 0)
			velocity = 0;
		transform += velocity * deltaTimeSeconds * direction;
	}

private:
	bool collected;
	float velocity;
	glm::vec3 direction;
	glm::vec3 target;

public:
	float maxSpeed;
	BuffType type;
	glm::vec3 baseColors[3];
};