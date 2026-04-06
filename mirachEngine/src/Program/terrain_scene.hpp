#pragma once

#include "scene.hpp"

#include "program.hpp"
#include "../ECS/entity_manager.hpp"
#include "../Physics/physics_system.hpp"
#include "../Graphics/Meshes/meshFile.hpp"
#include "../Graphics/Meshes/meshTerrain.hpp"
#include "../Graphics/Meshes/meshSkyDome.hpp"
#include "../Graphics/Animation/MD2.hpp"
#include "../Graphics/Animation/animation_system.hpp"

#include "../Terrain/terrain_manager.hpp"

#include "../Camera/camera.hpp"
#include "lua_wrapper.h"

#include "../Scripts/player_script.hpp"
#include "../Scripts/health.hpp"

#include "../Graphics/Rendering/render_system.hpp"

#include "../FSM/enemy.h"
#include "../FSM/EnemySystem.h"
#include "../FSM/enemyState.hpp"
#include "../FSM/hoverEnemyState.hpp"


/**
 * @class TerrainScene
 * @brief A complete game scene that initializes, runs, and manages an interactive terrain environment with ECS, physics, and rendering.
 *
 * This scene handles procedural terrain generation, model loading, entity creation, and a full game loop.
 * It integrates physics (ReactPhysics3D), MD2 animation playback, and FSM-based AI for enemies.
 */
class TerrainScene : public Scene 
{
public:
    /**
     * @brief Constructs a TerrainScene instance, initializing core subsystems.
     */
    TerrainScene();

    /**
     * @brief Runs the full scene lifecycle in the correct execution order.
     */
    void Run() override;

protected:
    /**
     * @brief Generates all CPU-side data such as terrain heightmaps and mesh geometry.
     */
    void GenerateCPUData() override;

    /**
     * @brief Initializes engine components like rendering, input, physics, and Lua bindings.
     */
    void InitialiseEngine() override;

    /**
     * @brief Uploads mesh and texture data to the GPU.
     */
    void UploadAssetsToGPU() override;

    /**
     * @brief Assigns graphics pipelines (shaders) to meshes.
     */
    void AssignPipelines() override;

    /**
     * @brief Creates and registers entities and components in the ECS.
     */
    void SetupECS() override;

    /**
     * @brief Executes the main loop: input, update, physics, animation, and rendering.
     */
    void MainLoop() override;

    /**
     * @brief Cleans up scene resources and systems before shutdown.
     */
    void CleanUp() override;

    void Shutdown() override;

private:
    Program m_app;                         ///< Manages the window, input, and rendering pipeline
    EntityManager m_entityManager;         ///< ECS entity/component manager
    PhysicsSystem m_physicsSystem;         ///< Physics world and collider system

    std::vector<MeshFile> m_meshFiles;     ///< Loaded 3D mesh files (e.g., props, models)

    MeshMD2 m_npc;                          ///< Animated MD2 robot mesh
    MeshMD2 m_firefly;                      ///< Animated MD2 firefly mesh

    MeshTerrain m_terrain;                 ///< Procedural terrain mesh
    MeshSkyDome m_sky;                     ///< Skydome for atmospheric background
    TerrainManager m_terrainManager;       ///< Manages terrain chunks and ECS registration

    Entity m_player;                       ///< Entity ID of the player character

    //EnemySystem mEnemySystem;           ///< [Optional] FSM-driven enemy system
    EnemyState m_enemyAI;                   ///< Enemy AI state machine instance
    HoverEnemyState m_hoverEnemyAI;

    /**
     * @brief Creates and registers the skydome entity in ECS.
     */
    void SetupSkyDomeEntity();

    /**
     * @brief Creates and registers the player character entity.
     */
    void SetupPlayer();

    /**
     * @brief Registers terrain chunks and their colliders with the ECS and physics system.
     */
    void RegisterTerrainToECS();

    /**
     * @brief Spawns robotic enemy NPCs with AI behavior.
     */
    void SpawnRobots();

    void SpawnFlatGround();

    /**
     * @brief Spawns cloth sacks as dynamic physics objects.
     */
    void SpawnSacks();

    void SpawnMassiveCrates();

    /**
     * @brief Spawns static or decorative tree models in the scene.
     */
    void SpawnTrees();

    /**
     * @brief Spawns crates, potentially breakable or interactable.
     */
    void SpawnCrates();

    /**
     * @brief Spawns firefly models, likely with animation or light effects.
     */
    void SpawnFireFlies();
};
