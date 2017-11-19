#include "GameWorld.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include <RacingGame/Geometry3D.h>
#include <RacingGame/Transform3D.h>
#include <Core/Managers/ResourcePath.h>


GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	// Meshes ----------------------------------------------------------------
	Mesh* mesh = new Mesh("terrain");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
	meshes[mesh->GetMeshID()] = mesh;

	mesh = new Mesh("sphere");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[mesh->GetMeshID()] = mesh;

	mesh = Geometry3D::CreateQuad("straight_road", 8.0f, glm::vec3(0.403, 0.4, 0.419));
	meshes[mesh->GetMeshID()] = mesh;
	
	// Shaders ---------------------------------------------------------------
	Shader *shader = new Shader("Terrain");
	shader->AddShader(RESOURCE_PATH::GAME_ROOT + "Shaders/TerrainShader.VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(RESOURCE_PATH::GAME_ROOT + "Shaders/TerrainDayCycleShader.FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;

	// Textures --------------------------------------------------------------
	TextureManager::LoadTexture(RESOURCE_PATH::GAME_ROOT + "Textures", "grass.jpg");
	TextureManager::LoadTexture(RESOURCE_PATH::GAME_ROOT + "Textures", "skybox.jpg");
	
	// Terrain ---------------------------------------------------------------
	terrain = new Terrain();
	terrain->SetMesh(meshes["terrain"]);
	terrain->SetShader(shaders["Terrain"]);
	terrain->SetTexture(TextureManager::GetTexture("grass.jpg"));
	terrain->SetBaseColor(glm::vec3(0.2, 0.592, 0.188));
	terrain->SetSize(Terrain::TerrainSize::TERRAIN_SIZE_MEDIUM);

	// Skybox ----------------------------------------------------------------
	skybox = new RenderableObject();
	skybox->SetMesh(meshes["sphere"]);
	skybox->SetShader(shaders["Terrain"]);
	skybox->SetTexture(TextureManager::GetTexture("skybox.jpg"));
	skybox->SetBaseColor(glm::vec3(0.211, 0.341, 0.827));
	skybox->SetModel(Transform3D::Scale(glm::vec3(250)));
	// Camera ----------------------------------------------------------------

	// Character -------------------------------------------------------------
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
}

void GameWorld::Update(float deltaTimeSeconds)
{
	// Update the positions of all objects and check for collisions
	Simulate(deltaTimeSeconds);

	RenderMesh(meshes["straight_road"], shaders["VertexColor"], Transform3D::Translate(glm::vec3(0.0f, 0.01f, 0.0f)));
}

void GameWorld::FrameEnd()
{
	//DrawCoordinatSystem();
	Render(terrain);
	Render(skybox);
}

void GameWorld::Render(RenderableObject *obj)
{
	Mesh *mesh = obj->GetMesh();
	Shader *shader = obj->GetShader();
	Texture2D *texture = obj->GetTexture();

	if (!mesh || !shader || !shader->program)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(obj->GetModel()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(shader->loc_textures[0], 0);

	glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(obj->GetBaseColor()));
	glUniform1f(glGetUniformLocation(shader->program, "time"), Engine::GetElapsedTime());

	mesh->Render();
}