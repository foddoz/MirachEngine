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
#include "../Graphics/UI/crossAim.hpp"
#include "../Graphics/Rendering/image_renderer.hpp"

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



class MurdochScripting;

/**
 * @class TerrainScene
 * @brief A complete game scene that initializes, runs, and manages an interactive terrain environment with ECS, physics, and rendering.
 *
 * This scene handles procedural terrain generation, model loading, entity creation, and a full game loop.
 * It integrates physics (ReactPhysics3D), MD2 animation playback, and FSM-based AI for enemies.
 */
class MurdochScene : public Scene, public std::enable_shared_from_this<MurdochScene> 
{
public:
    /**
     * @brief Constructs a scene in a room of the Murdoch campus, initializing core subsystems.
     */
    MurdochScene();

    /**
     * @brief Runs the full scene lifecycle in the correct execution order.
     */
    void Run() override;

    /**
     * @brief Register a function to be called every frame.
     */
    void RegisterUpdateCallback(std::function <void(float)> func);

    /*
     * @brief Remove a function from being called every frame.
     * Cannot be implemented unless a custom function type with operator== is implemented.
     */
    //void UnregisterProcessCallback(std::function <void()> func);

    EntityManager& GetECS();

    PhysicsSystem& GetPhysicsSystem();

    /**
     * @brief Spawn a dog bed at the specified position.
     */
    void SpawnDogBed(float x, float y, float z);

    /**
    * @brief Spawn a water bowl at the specified position.
    */
    void SpawnWaterBowl(float x, float y, float z);

    /**
    * @brief Spawn a food bowl at the specified position.
    */
    void SpawnFoodBowl(float x, float y, float z);

    /**
    * @brief Spawn a dog entity at the specified position
    */
    Entity SpawnDog(float x, float y, float z);

    void SetDogBreed(Entity dog, int breed);


    MeshFile& GetMeshFile(int index);

    MeshFile* GetMeshFile(const char* name);

    void AddDogToUpdate(Entity dog);

    void RemoveDogToUpdate(Entity dog);

    friend class Debug;

protected:
    /**
     * @brief Generates all CPU-side data such as mesh geometry.
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
     * @brief Executes the main loop: input, update, physics, and rendering.
     */
    void MainLoop() override;

    /**
     * @brief Cleans up scene resources and systems before shutdown.
     */
    void CleanUp() override;

    /**
     * @brief Shut down the program
     */ 
    void Shutdown() override;

    

private:
    Program m_app;                         ///< Manages the window, input, and rendering pipeline
    EntityManager m_entityManager;         ///< ECS entity/component manager
    PhysicsSystem m_physicsSystem;         ///< Physics world and collider system

    std::vector<MeshFile> m_meshFiles;     ///< Loaded 3D mesh files (e.g., props, models)

    MeshSkyDome m_sky;                     ///< Skydome for atmospheric background

    Entity m_player;                       ///< Entity ID of the player character

    CrossAim m_aim;        /// Player's A

    Texture m_texture{ TextureTarget::Texture2D };
    ImageRenderer m_image2D;

    Entity m_testRigidBodyEntity;
    std::vector<Entity> m_testColliderEntities;

    Entity m_COMEntity;
    Entity m_posEntity;

    std::vector<std::function<void(float)>> updateCallbacks;

    InputManager m_inputManager;

    MurdochScripting* m_scripting;

    std::unordered_map<std::string, int> m_meshFileMap;

    std::set<Entity> m_dogs;
    
    /**
    * @brief Render dog in 3D world.
    */
    void RenderAllDogs();
    /**
     * @brief Creates and registers the skydome entity in ECS.
     */
    void SetupSkyDomeEntity();

    /**
     * @brief Creates and registers the player character entity.
     */
    void SetupPlayer();

    /**
     * @brief Creates and registers a flat ground collider/mesh for the player and objects to stand on.
     */
    void SpawnFlatGround();

    /**
     * @brief Spawns a collection of cloth sacks as dynamic physics objects in preset positions.
     */
    void SpawnSacks();

    /**
     * @brief Spawn a single cloth sack at the given position.
     *
     * @param position World-space position where the sack should be spawned.
     */
    void SpawnSack(const Vector3f& position);

    /**
     * @brief Spawn a set of spherical balls for physics testing or gameplay.
     */
    void SpawnBalls();

    /**
     * @brief Spawn a single ball at the specified world-space position.
     *
     * @param position World-space position for the ball.
     */
    void SpawnBall(const Vector3f& position);

    /**
     * @brief Spawn a chair object at the given position.
     *
     * @param position World-space position for the chair entity.
     */
    void SpawnChair(const Vector3f& position);

    /**
     * @brief Spawn the main map layout (static geometry, buildings, etc.).
     */
    void SpawnMap();

    /**
     * @brief Spawn a snowman entity at the given position.
     *
     * @param position World-space position for the snowman.
     */
    void SpawnSnowman(const Vector3f& position);

    /**
     * @brief Attach spherical bounding volume(s) to an entity and register them with physics.
     *
     * @param e        Entity to which the bounds belong.
     * @param localTF  Local transform of the bounds relative to the entity.
     * @param mass     Mass to assign to the rigid body associated with the bounds.
     * @param testing  Whether this is used for debugging/testing purposes.
     */
    void AddSphericalBounds(Entity e, const Transform& localTF, float mass, bool testing);

    /**
     * @brief Add axis-aligned bounding volumes to an existing physics body.
     *
     * @param body            Pointer to the physics rigid body.
     * @param boundsPosition  Local/world position of the bounds (implementation-dependent).
     * @param boundsScale     Scale (half-extents or full size) of the bounds.
     * @param testing         Whether the bounds are for testing/debug visualization.
     */
    void AddBounds(Physics::RigidBody* body, const Vector3f& boundsPosition, const Vector3f& boundsScale, bool testing);

    /**
     * @brief Add bounding volumes to an entity and create/configure its rigid body.
     *
     * @param e        Entity that will receive the bounds.
     * @param localTF  Local transform of the bounds relative to the entity.
     * @param mass     Mass of the rigid body to create/assign.
     * @param testing  Whether this setup is used for testing/debugging.
     */
    void AddBounds(Entity e, const Transform& localTF, float mass, bool testing);

    /**
     * @brief Spawns a set of crates in the scene, potentially breakable or interactable.
     */
    void SpawnCrates();

    /**
     * @brief Spawn a single crate at the specified position.
     *
     * @param position World-space position for the crate.
     */
    void SpawnCrate(const Vector3f& position);

    /**
     * @brief Spawn a bin (trash can) entity at the given position using a mesh file index.
     *
     * @param position  World-space position of the bin.
     * @param fileIndex Index into the mesh file array specifying the bin mesh.
     */
    void SpawnBin(const Vector3f& position, int fileIndex);

    /**
     * @brief Set up any debugging or test colliders associated with the given entity.
     *
     * @param e Entity that will receive test colliders.
     */
    void SetupTestColliders(Entity e);

    /**
     * @brief Update positions/transforms of any test colliders to match their parent entities.
     */
    void UpdateTestColliders();
};
