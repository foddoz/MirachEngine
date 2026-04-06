#include "entity_manager.hpp"

#include "ScriptingAPI/scripting_api.hpp"

EntityManager::EntityManager() 
	: 
	m_nextEntityID(1) 
{

}

Entity EntityManager::CreateEntity() 
{
    Entity e = m_nextEntityID++;
    m_entities.push_back(e);
    return e;
}

void EntityManager::DestroyEntity(Entity e) 
{
    // Remove entity from the entity list
    m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end());

    // Loop through all component managers and remove this entity's components
    for (auto& [_, base] : m_componentManagers) 
    {
        base->RemoveComponent(e);
    }

    // Optional: mark entity as invalid in caller context (note: has no effect here)
    e = INVALID_ENTITY;
}

const std::vector<Entity>& EntityManager::GetAllEntities() const 
{
    return m_entities;
}

void EntityManager::RegisterScriptingAPI() 
{

}

void EntityManager::PrintRegisteredComponents() const
{
    std::cout << "== Registered Component Types ==" << std::endl;
    for (const auto& [ti, ptr] : m_componentManagers)
    {
        std::cout << ti.name() << " at " << ptr.get() << std::endl;
    }
}
