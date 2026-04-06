#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "../../Camera/camera.hpp"
#include "../../Transform/transform.hpp"
#include "../../Program/program.hpp"

#include "mesh_renderer.hpp"
#include "../../ECS/entity_manager.hpp"

#include "../graphics.hpp"

/**
 * @brief Core ECS-based rendering system.
 *
 * Iterates over all entities that contain a MeshRenderer and Transform component,
 * and renders each using its assigned shader pipeline and transform.
 * Applies frustum culling using the provided camera, binds the appropriate
 * vertex arrays and textures, and issues draw calls.
 *
 * @param gApp Reference to the main Program instance holding global GPU state.
 * @param camEntity The entity containing the active camera component.
 * @param entityManager Reference to the ECS EntityManager for accessing components.
 */
void RenderSystem(const Program& gApp, const Entity& camEntity, EntityManager& entityManager);

/**
 * @brief Renders a skybox or skydome mesh as a background element.
 *
 * This should be called before rendering opaque geometry.
 * It disables depth writing to ensure it is always behind other objects.
 *
 * @param gApp Reference to the main Program instance for shader control.
 * @param cam Reference to the Camera used for view-projection matrix.
 * @param transform Transform of the skybox/skydome entity.
 * @param renderer MeshRenderer containing the mesh and texture data.
 */
void RenderSkybox(const Program& gApp, const Camera& cam, const Transform& transform, const MeshRenderer& renderer);

#endif // RENDERSYSTEM_HPP
