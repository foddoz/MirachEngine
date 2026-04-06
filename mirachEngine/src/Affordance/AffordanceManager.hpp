#pragma once
#include "affordance.hpp"
#include <unordered_set>

class AffordanceManager 
{
public:
    void Register(Entity e, const Affordance& a);
    void Unregister(Entity e);

    Entity FindNearest(Wellness::Component comp, const Vector3f& origin, float maxDist) const;

    bool TryReserve(Entity affordanceEntity, Entity npc);
    void Release(Entity affordanceEntity, Entity npc);

private:
    struct AffordanceRecord 
    {
        Affordance data;
        Vector3f   position;
        unsigned int limit = 1;
        unsigned int count = 0;
        std::unordered_set<Entity> currentUser; // Current User
    };

    std::unordered_map<Entity, AffordanceRecord> m_affordanceTable; 
    std::unordered_map<Wellness::Component, std::vector<Entity>> m_wellnessTable; 
};