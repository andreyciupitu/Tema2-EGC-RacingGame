#pragma once

#include <string>
#include <vector>

#include <Component/SimpleScene.h>
#include <RacingGame/RenderableObject.h>
#include <RacingGame/Car.h>
#include <RacingGame/Terrain.h>
#include <RacingGame/Road.h>
#include <RacingGame/Camera.h>
#include <RacingGame/Skybox.h>
#include <RacingGame/Buff.h>

class GameWorld : public World
{
public:
	GameWorld() {}
	virtual ~GameWorld();
	virtual void Init() override;

private:
	virtual void FrameStart() override;
	void Simulate(float deltaTimeSeconds);
	virtual void Update(float deltaTimeSeconds) override;
	virtual void FrameEnd() override;

	void Render(RenderableObject *obj);
	void RenderWheels();
	void GeneratePickup(int i, bool newRoad = false);

	virtual void OnKeyPress(int key, int mods) override;
	virtual void OnKeyRelease(int key, int mods) override;
	virtual void OnInputUpdate(float deltaTime, int mods) override;
	virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

private:
	Camera *camera;
	Skybox *skybox;
	Terrain *terrain;
	Road *road;
	std::vector<Buff*> pickups;
	Car *player;
	unsigned int drawMode;
	bool firstPersonCamera;
	float friction;

	SpatialObject* wheels[4];
};