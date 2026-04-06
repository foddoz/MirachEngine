#pragma once
#include "enemy.h"

/**
 * @brief System class responsible for updating all Enemy entities.
 *
 * This system queries all entities that have an Enemy component and
 * updates them each frame by calling their individual `Update()` methods.
 * It is designed to work with an ECS (Entity-Component-System) framework.
 */
class EnemySystem 
{
private:
    /// List of entity IDs that have an Enemy component.
    std::vector<Entity> m_enemies;

    /// Reference to the entity manager used to access components.
    EntityManager& m_entityManager;

public:
    /**
     * @brief Constructor. Automatically collects all entities with Enemy components.
     * @param em Reference to the global EntityManager.
     */
    EnemySystem(EntityManager& em) 
        : 
        m_entityManager(em)
    {
        const auto& allEnemies = em.GetAllComponents<Enemy*>();
        for (const auto& [entity, enemyPtr] : allEnemies) 
        {
            if (enemyPtr) 
            {
                m_enemies.push_back(entity);
            }
        }
    }

    /*
    /// (Optional) Manual registration of a new Enemy pointer (if not using ECS auto-collection).
    void Register(Enemy* e) {
        enemies.push_back(e);
    }
    */

    /**
     * @brief Updates all registered Enemy entities.
     * Calls the `Update()` method of each Enemy, passing the delta time.
     * @param dt The time elapsed since the last frame (delta time).
     */
    void Update(float dt) 
    {
        for (Entity e : m_enemies) 
        {
            if (m_entityManager.HasComponent<Enemy*>(e)) 
            {
                Enemy* enemy = m_entityManager.GetComponent<Enemy*>(e);
                if (enemy) 
                {
                    enemy->Update(dt);
                }
            }
        }
    }

    /**
     * @brief Destructor.
     * Does not delete any enemy components, as memory is managed by the ECS.
     */
    ~EnemySystem() = default;
};
