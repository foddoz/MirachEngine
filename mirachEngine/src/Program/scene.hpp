#pragma once

/**
 * @class Scene
 * @brief Abstract base class for defining and managing a game scene lifecycle.
 *
 * This class enforces a fixed execution order for initializing, running,
 * and cleaning up a scene, and must be subclassed to provide specific behavior.
 */
class Scene 
{
public:
    /**
     * @brief Default constructor.
     */
    Scene() = default;

    /**
     * @brief Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~Scene() = default;

    /**
     * @brief Runs the full scene lifecycle in a fixed execution order.
     *
     * Calls the following methods in sequence:
     * GenerateCPUData -> InitialiseEngine -> UploadAssetsToGPU ->
     * AssignPipelines -> SetupECS -> MainLoop -> CleanUp
     */
    virtual void Run() = 0;

protected:
    /**
     * @brief Performs CPU-side data generation such as procedural meshes, textures, etc.
     */
    virtual void GenerateCPUData() = 0;

    /**
     * @brief Initializes core engine components (e.g., renderer, physics, input).
     */
    virtual void InitialiseEngine() = 0;

    /**
     * @brief Uploads assets like meshes and textures to the GPU.
     */
    virtual void UploadAssetsToGPU() = 0;

    /**
     * @brief Assigns graphics pipelines or shader programs to mesh renderers.
     */
    virtual void AssignPipelines() = 0;

    /**
     * @brief Sets up the ECS by creating entities and attaching components.
     */
    virtual void SetupECS() = 0;

    /**
     * @brief Executes the main game loop (input, update, render).
     */
    virtual void MainLoop() = 0;

    /**
     * @brief Releases all resources and cleans up the scene.
     */
    virtual void CleanUp() = 0;

    virtual void Shutdown() = 0;
};
