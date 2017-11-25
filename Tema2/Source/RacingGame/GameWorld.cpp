#include "GameWorld.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include <RacingGame/Geometry3D.h>
#include <RacingGame/Transform.h>
#include <Core/Managers/ResourcePath.h>
#include <Component/Transform/Transform.h>


GameWorld::~GameWorld()
{
	for (auto w : wheels)
		delete w;
	for (auto b : pickups)
		delete b;
	delete terrain;
	delete skybox;
	delete camera;
	delete player;
	delete road;
}

void GameWorld::Init()
{
	// sets common GL states
	glClearColor(0, 0, 0, 1);

	// Parameters ------------------------------------------------------------
	int drawDistance = 20;
	drawMode = GL_TRIANGLES;
	firstPersonCamera = false;
	friction = -0.5f;

	// Meshes ----------------------------------------------------------------
	MeshManager::LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj", "terrain");
	MeshManager::LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj", "sphere");
	MeshManager::LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj", "box");
	MeshManager::LoadMesh(RESOURCE_PATH::GAME_ROOT + "Models", "carvox.obj", "car");
	MeshManager::LoadMesh(RESOURCE_PATH::GAME_ROOT + "Models", "centered_wheel.obj", "wheel");

	// Shaders ---------------------------------------------------------------
	ShaderManager::LoadShader(RESOURCE_PATH::GAME_ROOT + "Shaders/TerrainShader.VS.glsl",
		RESOURCE_PATH::GAME_ROOT + "Shaders/TerrainDayCycleShader.FS.glsl", "Terrain");

	// Textures --------------------------------------------------------------
	TextureManager::LoadTexture(RESOURCE_PATH::GAME_ROOT + "Textures", "green_grass.jpg");
	TextureManager::LoadTexture(RESOURCE_PATH::GAME_ROOT + "Textures", "skybox.jpg");
	TextureManager::LoadTexture(RESOURCE_PATH::GAME_ROOT + "Textures", "asphalt.jpg");
	
	// Terrain ---------------------------------------------------------------
	terrain = new Terrain();
	terrain->SetMesh(MeshManager::GetMesh("terrain"));
	terrain->SetShader(ShaderManager::GetShader("Terrain"));
	terrain->SetTexture(TextureManager::GetTexture("green_grass.jpg"));
	terrain->SetBaseColor(glm::vec3(0.2, 0.592, 0.188));
	terrain->SetSize(Terrain::TerrainSize::TERRAIN_SIZE_LARGE);

	// Skybox ----------------------------------------------------------------
	skybox = new Skybox(glm::vec3(0), 250.0f);
	skybox->SetMesh(MeshManager::GetMesh("sphere"));
	skybox->SetShader(ShaderManager::GetShader("Terrain"));
	skybox->SetTexture(TextureManager::GetTexture("skybox.jpg"));
	skybox->SetBaseColor(glm::vec3(0.211, 0.341, 0.827));

	// Character -------------------------------------------------------------
	player = new Car();
	player->SetMesh(MeshManager::GetMesh("car"));
	player->GetMesh()->UseMaterials(true);
	player->SetScale(glm::vec3(0.05f)); // mesh is too large
	player->SetMaxSpeed(5.0f);

	// Wheels ----------------------------------------------------------------
	Geometry3D::BoundingBox box = player->GetBoundingBox();
	float length = box.maxX - box.minX;
	float width = box.maxZ - box.minZ;

	wheels[0] = new SpatialObject();
	wheels[0]->SetMesh(MeshManager::GetMesh("wheel"));
	wheels[0]->GetMesh()->UseMaterials(true);
	wheels[0]->SetScale(glm::vec3(0.02f));
	wheels[0]->SetTransform(glm::vec3(-length / 4, 0.1f, -width / 2 - 0.1f));
	wheels[0]->SetRotation(glm::vec3(0, M_PI / 2, 0));

	wheels[1] = new SpatialObject();
	wheels[1]->SetMesh(MeshManager::GetMesh("wheel"));
	wheels[1]->GetMesh()->UseMaterials(true);
	wheels[1]->SetScale(glm::vec3(0.02f));
	wheels[1]->SetTransform(glm::vec3(length / 4, 0.1f, -width / 2 - 0.1f));
	wheels[1]->SetRotation(glm::vec3(0, M_PI / 2, 0));

	wheels[2] = new SpatialObject();
	wheels[2]->SetMesh(MeshManager::GetMesh("wheel"));
	wheels[2]->GetMesh()->UseMaterials(true);
	wheels[2]->SetScale(glm::vec3(0.02f));
	wheels[2]->SetTransform(glm::vec3(length / 4, 0.1f, width / 2 + 0.1f));
	wheels[2]->SetRotation(glm::vec3(0, M_PI / 2, 0));

	wheels[3] = new SpatialObject();
	wheels[3]->SetMesh(MeshManager::GetMesh("wheel"));
	wheels[3]->GetMesh()->UseMaterials(true);
	wheels[3]->SetScale(glm::vec3(0.02f));
	wheels[3]->SetTransform(glm::vec3(-length / 4, 0.1f, width / 2 + 0.1f));
	wheels[3]->SetRotation(glm::vec3(0, M_PI / 2, 0));

	// Camera ----------------------------------------------------------------
	camera = new Camera(glm::vec3(-3, 1, 0), player->GetTransform(), glm::vec3(0, 1, 0));
	camera->projection = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 250.0f);

	// Road ------------------------------------------------------------------
	road = new Road(glm::vec3(0, 0.01f, 0), 6.0f, drawDistance);
	road->SetTexture(TextureManager::GetTexture("asphalt.jpg"));
	road->SetShader(ShaderManager::GetShader("Terrain"));

	// Pickups ---------------------------------------------------------------
	for (int i = 0; i < 4; i++)
	{
		Buff *tmp = new Buff(static_cast<Buff::BuffType>(rand() % 3));
		tmp->SetMesh(MeshManager::GetMesh("sphere"));
		tmp->SetShader(ShaderManager::GetShader("Color"));
		tmp->SetScale(glm::vec3(0.3f));
		pickups.push_back(tmp);
		GeneratePickup(i, true);
	}

	// Default rendering mode will use depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void GameWorld::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets the screen area where to draw
	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void GameWorld::Simulate(float deltaTimeSeconds)
{
	// Generate more road segments
	road->Update(camera->position, camera->forward);

	// Update the player coordinates
	player->Update(deltaTimeSeconds);

	if (player->GetAcceleration() == friction) // no acceleration
		camera->position += player->GetVelocity() * player->GetDirection() * deltaTimeSeconds;

	// Spawn borders and pick-ups;
	std::vector<glm::vec3> points = road->GetKeyPoints();
	
	for (int i = 0; i < pickups.size(); i++)
	{
		// If out of view
		if (glm::dot(player->GetDirection(), pickups[i]->GetTransform() - camera->position) < 0)
			GeneratePickup(i);
		// Make sure the moving buffs keep moving
		if (pickups[i]->type == Buff::BuffType::LIFE)
			pickups[i]->Seek(points[13] + glm::vec3(0, 0.2f, 0));
		pickups[i]->Update(deltaTimeSeconds);
	}

	// Rotate the wheels
	if (player->GetAcceleration() > 0)
		for (int i = 0; i < 4; i++)
		{
			glm::vec3 xUpdate = wheels[i]->GetRotation() + deltaTimeSeconds * player->GetAcceleration() * glm::vec3(1, 0, 0);
			wheels[i]->SetRotation(xUpdate);
		}

	// Check for collisions
	for (auto border : road->GetRoadBorders())
	{
		if (Geometry3D::CheckCollision(player, border))
		{
			player->OnCollision(border);

			// Spawn player in the middle of the road
			player->SetTransform(points[2]);
			camera->position = points[2] - 3.0f * player->GetDirection() + camera->up;
			camera->Set(camera->position, player->GetTransform(), glm::vec3(0, 1, 0));
			firstPersonCamera = false;
		}

		// Keeps the balls on the road ---- MIGHT CAUSE FPS DROPS
		for (auto buff : pickups)
			if (!buff->IsCollected() && Geometry3D::CheckCollision(border, buff) && buff->type == Buff::BuffType::LIFE)
				buff->Despawn();
	}
	for (auto buff : pickups)
		if (!buff->IsCollected() && Geometry3D::CheckCollision(player, buff))
		{
			player->OnCollision(buff);
			buff->Despawn();
		}
}

void GameWorld::Update(float deltaTimeSeconds)
{
	// Update the positions of all objects and check for collisions
	Simulate(deltaTimeSeconds);

	// Keep the skybox stationary to the observer
	skybox->SetCenter(camera->position);

	// Render the scene
	for (auto obj : road->GetRoadBorders())
		Render(obj);
	for (auto obj : pickups)
		if (!obj->IsCollected()) // don't render collected buffs
			Render(obj);
	Render(road);
	Render(terrain);
	Render(skybox);
	Render(player);
	RenderWheels();
}

void GameWorld::FrameEnd()
{
	if (player->score > 1000)
	{
		std::cout << "You win!" << std::endl;
		std::cout << "Your score was: " << player->score << std::endl;
		getchar();
		window->Close();
	}
	if (player->lives <= 0)
	{
		std::cout << "You lose!" << std::endl;
		std::cout << "Your score was: " << player->score << std::endl;
		getchar();
		window->Close();
	}
}

void GameWorld::RenderWheels()
{
	for (int i = 0; i < 4; i++)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix *= Transform::Translate(player->GetTransform());
		modelMatrix *= Transform::RotateOY(player->GetRotation().y + wheels[i]->GetRotation().y);
		modelMatrix *= Transform::Translate(wheels[i]->GetTransform());
		modelMatrix *= Transform::RotateOX(wheels[i]->GetRotation().x);
		modelMatrix *= Transform::Scale(wheels[i]->GetScale());

		Mesh *mesh = wheels[i]->GetMesh();
		Shader *shader = wheels[i]->GetShader();
		Texture2D *texture = wheels[i]->GetTexture();

		if (!mesh || !shader || !shader->program)
			return;

		shader->Use();
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(shader->loc_textures[0], 0);

		glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(wheels[i]->GetBaseColor()));
		glUniform1f(glGetUniformLocation(shader->program, "time"), Engine::GetElapsedTime());

		mesh->SetDrawMode(drawMode);
		mesh->Render();
	}
}

void GameWorld::Render(RenderableObject *obj)
{
	Mesh *mesh = obj->GetMesh();
	Shader *shader = obj->GetShader();
	Texture2D *texture = obj->GetTexture();

	if (!mesh || !shader || !shader->program)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(obj->GetModel()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(shader->loc_textures[0], 0);

	glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(obj->GetBaseColor()));
	glUniform1f(glGetUniformLocation(shader->program, "time"), Engine::GetElapsedTime());

	mesh->SetDrawMode(drawMode);
	mesh->Render();
}

void GameWorld::GeneratePickup(int i, bool newRoad)
{
	std::vector<glm::vec3> points = road->GetKeyPoints();
	int roadIndex = 0;

	// Respawn at the end of the current road
	if (newRoad)
		roadIndex = (i + 1) * pickups.size();
	else
		roadIndex = pickups.size() * pickups.size();

	// Randomly generate a new type
	pickups[i]->Spawn();
	Buff::BuffType type = static_cast<Buff::BuffType>(rand() % 3);
	pickups[i]->SetBaseColor(pickups[i]->baseColors[type]);
	pickups[i]->type = type;
	pickups[i]->maxSpeed = 0.0f;

	// Generate a position
	float w = road->GetWidth();
	glm::vec3 offset = glm::vec3(w / 4, 0.2f, w / 4) * glm::vec3(rand() % 3 - 1, 1, rand() % 3 - 1);
	switch (type)
	{
		// Life buffs spawn at the middle of the road
	case Buff::BuffType::LIFE: 
		offset = glm::vec3(0, 0.2f, 0);
		pickups[i]->SetTransform(points[10] + offset);
		pickups[i]->maxSpeed = 3.0f;

		// Go to a point on the road
		pickups[i]->Seek(points[13] + glm::vec3(0, 0.2f, 0));
		break;
	case Buff::BuffType::SLOW:
		pickups[i]->SetTransform(points[roadIndex] + offset);
		break;
	case Buff::BuffType::SPEEDUP:
		pickups[i]->SetTransform(points[roadIndex] + offset);
		break;
	}
}

void GameWorld::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_UP)
		player->SetAcceleration(1);
	if (key == GLFW_KEY_DOWN)
		player->SetAcceleration(-4);
	if (key == GLFW_KEY_LEFT)
		player->SetAngularVelocity(glm::vec3(0, 1, 0));
	if (key == GLFW_KEY_RIGHT)
		player->SetAngularVelocity(glm::vec3(0, -1, 0));

	// Change camera view
	if (key == GLFW_KEY_V)
	{
		firstPersonCamera = !firstPersonCamera;
		if (firstPersonCamera)
			camera->MoveForward(camera->distanceToTarget);
		else
			camera->MoveForward(-camera->distanceToTarget);
	}

	// Change render mode
	if (key == GLFW_KEY_SPACE)
	{
		if (drawMode == GL_TRIANGLES)
			drawMode = GL_LINES;
		else
			drawMode = GL_TRIANGLES;
	}
}

void GameWorld::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_UP)
		player->SetAcceleration(friction); // the car has inertia
	if (key == GLFW_KEY_DOWN)
		player->SetAcceleration(friction);
	if (key == GLFW_KEY_LEFT)
		player->SetAngularVelocity(glm::vec3(0));
	if (key == GLFW_KEY_RIGHT)
		player->SetAngularVelocity(glm::vec3(0));
}

void GameWorld::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (firstPersonCamera)
		{
			camera->RotateFirstPerson_OX(-0.01f * (float)deltaY);
			camera->RotateFirstPerson_OY(-0.01f * (float)deltaX);
		}
		else
		{
			camera->RotateThirdPerson_OX(-0.01f * (float)deltaY);
			camera->RotateThirdPerson_OY(-0.01f * (float)deltaX);
		}
	}
}

void GameWorld::OnInputUpdate(float deltaTime, int mods)
{
	// Camera controls
	if (window->KeyHold(GLFW_KEY_UP) || window->KeyHold(GLFW_KEY_DOWN))
		camera->position += player->GetVelocity() * player->GetDirection() * deltaTime;
	if (window->KeyHold(GLFW_KEY_LEFT) || window->KeyHold(GLFW_KEY_RIGHT))
	{
		float speed = glm::normalize(player->GetAngularVelocity()).y;
		if (firstPersonCamera)
			camera->RotateFirstPerson_OY(speed * deltaTime);
		else
			camera->RotateThirdPerson_OY(speed * deltaTime);
	}
}