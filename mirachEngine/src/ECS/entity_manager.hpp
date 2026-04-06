#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "ecs_types.hpp"
#include "component_manager.hpp"

/**
 * @class EntityManager
 * @brief Manages entity creation, destruction, and component access in the ECS system.
 *
 * This class handles creation of unique entity IDs, storage of active entities,
 * and dynamic registration and access of typed component managers.
 */
class EntityManager 
{
public:

    /**
     * @brief Constructs a new EntityManager instance with ID counter initialized.
     */
    EntityManager();

    /**
     * @brief Creates a new unique entity.
     * @return The created entity ID.
     */
    Entity CreateEntity();

    /**
     * @brief Destroys an entity and removes all its associated components.
     * @param e The entity to destroy.
     */
    void DestroyEntity(Entity e);

    /**
     * @brief Returns a list of all currently active entities.
     * @return Vector of active entity IDs.
     */
    const std::vector<Entity>& GetAllEntities() const;

    static void RegisterScriptingAPI();

    // ===================== Component Management API ===================== //

    /**
     * @brief Adds a component of type T to the given entity.
     * @tparam T The component type.
     * @param e The entity to attach the component to.
     * @param component The component instance to copy.
     */
    template<typename T>
    void AddComponent(Entity e, const T& component)
    {
        GetOrCreateManager<T>().AddComponent(e, component);
    }
    /**
     * @brief Removes a component of type T from the given entity.
     * @tparam T The component type.
     * @param e The entity to remove the component from.
     */
    template<typename T>
    void RemoveComponent(Entity e) 
    {
        GetOrCreateManager<T>().RemoveComponent(e);
    }

    /**
     * @brief Checks if an entity has a component of type T.
     * @tparam T The component type.
     * @param e The entity to check.
     * @return True if the entity has the component, false otherwise.
     */
    template<typename T>
    bool HasComponent(Entity e) const 
    {
        return GetManager<T>().HasComponent(e);
    }

    /**
     * @brief Retrieves a reference to the component of type T for an entity.
     * @tparam T The component type.
     * @param e The entity owning the component.
     * @return Reference to the component.
     * @throws std::runtime_error if the component does not exist.
     */
    template<typename T>
    T& GetComponent(Entity e) 
    {
        return GetManager<T>().GetComponent(e);
    }

    /**
     * @brief Checks if any components of type T are registered in the system.
     * @tparam T The component type.
     * @return True if the component type exists, false otherwise.
     */
    template<typename T>
    bool HasComponentType() const 
    {
        return m_componentManagers.find(std::type_index(typeid(T))) != m_componentManagers.end();
    }

    /**
     * @brief Returns a reference to the map of all components of type T.
     * @tparam T The component type.
     * @return Mutable reference to the entity-component map.
     */
    template<typename T>
    std::unordered_map<Entity, T>& GetAllComponents() 
    {
        return GetOrCreateManager<T>().GetAll();
    }

    /**
     * @brief Returns a const reference to the map of all components of type T.
     * @tparam T The component type.
     * @return Const reference to the entity-component map.
     */
    template<typename T>
    const std::unordered_map<Entity, T>& GetAllComponents() const 
    {
        std::type_index ti(typeid(T));
        auto it = m_componentManagers.find(ti);
        if (it == m_componentManagers.end()) 
        {
            static const std::unordered_map<Entity, T> empty;
            return empty;
        }
        return static_cast<const ComponentManagerWrapper<T>*>(it->second.get())->manager.GetAll();
    }

    template<typename T>
    Entity FindEntityByComponent(const T& comp) const 
    {
        // If caller passed a pointer component type (e.g., RigidBody*), comp may be null.
        if constexpr (std::is_pointer_v<T>) 
        {
            if (comp == nullptr) return INVALID_ENTITY;
        }

        // IMPORTANT: T must match how you store the component:
        //   - if manager stores RigidBody*  -> T must be RigidBody*
        //   - if manager stores Transform   -> T must be Transform
        const auto& map = GetAllComponents<T>();

        if constexpr (std::is_pointer_v<T>) 
        {
            // Stored values are pointers; compare pointer values
            for (const auto& [e, p] : map) 
            {
                if (p == comp)
                {
                    return e;
                }
            }
        }
        else 
        {
            // Stored values are objects; compare addresses of the stored object
            for (const auto& [e, c] : map) 
            {
                if (&c == &comp)
                {
                    return e; // comp must be a reference to the stored component (not a copy)
                }
            }
        }

        return INVALID_ENTITY;
    }

    /**
     * @brief Prints all registered component types and their internal pointers.
     *        Useful for debugging component registration.
     */
    void PrintRegisteredComponents() const;

private:
    Entity m_nextEntityID;                      ///< Next available entity ID.
    std::vector<Entity> m_entities;             ///< List of all active entity IDs.

    // ===================== Component Manager Infrastructure ===================== //

    /**
     * @brief Abstract base class for all component manager wrappers.
     */
    struct IComponentManagerBase 
    {
        virtual ~IComponentManagerBase() = default;
        virtual void RemoveComponent(Entity) = 0;
    };

    /**
     * @brief Concrete wrapper for a strongly typed component manager.
     * @tparam T Component type.
     */
    template<typename T>
    struct ComponentManagerWrapper : IComponentManagerBase 
    {
        ComponentManager<T> manager;

        void RemoveComponent(Entity e) override 
        {
            manager.RemoveComponent(e);
        }
    };

    std::unordered_map<std::type_index, std::unique_ptr<IComponentManagerBase>> m_componentManagers; ///< Map of component types to their managers.

    // ===================== Internal Accessors ===================== //

    /**
     * @brief Returns the manager for the given component type, creating it if needed.
     * @tparam T Component type.
     * @return Reference to the component manager.
     */
    template<typename T>
    ComponentManager<T>& GetOrCreateManager() 
    {
        std::type_index ti = std::type_index(typeid(T));
        if (m_componentManagers.find(ti) == m_componentManagers.end()) 
        {
            m_componentManagers[ti] = std::make_unique<ComponentManagerWrapper<T>>();
        }
        return static_cast<ComponentManagerWrapper<T>*>(m_componentManagers[ti].get())->manager;
    }

    /**
     * @brief Gets the component manager for a type (const).
     * @tparam T Component type.
     * @return Const reference to the component manager.
     * @throws std::runtime_error if the manager doesn't exist.
     */
    template<typename T>
    const ComponentManager<T>& GetManager() const 
    {
        std::type_index ti = std::type_index(typeid(T));
        auto it = m_componentManagers.find(ti);
        if (it == m_componentManagers.end()) 
        {
            throw std::runtime_error("Component type not registered.");
        }
        return static_cast<ComponentManagerWrapper<T>*>(it->second.get())->manager;
    }

    /**
     * @brief Gets the component manager for a type (non-const).
     * @tparam T Component type.
     * @return Reference to the component manager.
     */
    template<typename T>
    ComponentManager<T>& GetManager() 
    {
        return const_cast<ComponentManager<T>&>(static_cast<const EntityManager*>(this)->GetManager<T>());
    }
};
#endif
