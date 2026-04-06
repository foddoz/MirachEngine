#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include "ecs_types.hpp"
#include <unordered_map>

/**
 * @brief Generic ECS Component Manager that maps entities to component data.
 *
 * @tparam ComponentType The type of component managed (e.g., Transform, MeshRenderer).
 */
template<typename ComponentType>
class ComponentManager 
{
public:

    /**
     * @brief Adds a component to the specified entity.
     *
     * @param e The entity to assign the component to.
     * @param component The component data to store.
     */
    void AddComponent(Entity e, const ComponentType& component) 
    {
        m_components[e] = component;
    }

    /**
     * @brief Removes the component associated with an entity.
     *
     * @param e The entity whose component should be removed.
     */
    void RemoveComponent(Entity e) 
    {
        m_components.erase(e);
    }

    /**
     * @brief Checks if an entity has a component of this type.
     *
     * @param e The entity to check.
     * @return true if the component exists, false otherwise.
     */
    bool HasComponent(Entity e) const 
    {
        return m_components.find(e) != m_components.end();
    }

    /**
     * @brief Retrieves a reference to the component of an entity.
     *
     * @param e The entity whose component to retrieve.
     * @return Reference to the component.
     */
    ComponentType& GetComponent(Entity e) 
    {
        return m_components.at(e);
    }

    std::unordered_map<Entity, ComponentType>& GetAll() 
    { 
        return m_components; 
    }

    /**
     * @brief Returns all stored components for iteration or access.
     *
     * @return Const reference to the internal unordered map.
     */
    const std::unordered_map<Entity, ComponentType>& GetAll() const 
    {
        return m_components;
    }

private:
    std::unordered_map<Entity, ComponentType> m_components; ///< Map of Entity -> Component.
};

#endif
