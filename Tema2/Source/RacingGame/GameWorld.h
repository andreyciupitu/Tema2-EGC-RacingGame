#pragma once

#include <string>

#include <Component/SimpleScene.h>
#include <RacingGame/RenderableObject.h>
#include <RacingGame/Character.h>
#include <RacingGame/Terrain.h>

class GameWorld : public SimpleScene
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

	virtual void OnKeyPress(int key, int mods) override {};
	virtual void OnInputUpdate(float deltaTime, int mods) override {};

private:
	RenderableObject *skybox;
	Terrain *terrain;
};