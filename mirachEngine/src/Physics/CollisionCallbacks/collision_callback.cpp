#include "../physics.hpp"

#include <unordered_map>

namespace Physics
{
    CollisionCallback::CollisionCallback(RigidBody* rb)
        : 
        m_body(rb)
    {

    }

    void CollisionCallback::Update(std::vector<ContactManifold> contactManifolds)
    {
        std::unordered_map<RigidBody*, ContactManifold> current;

        for (const ContactManifold& m : contactManifolds)
        {
            if (m.colA->GetParentBody() == m_body)
            {
                current.emplace(m.colB->GetParentBody(), m);
            }
            else if (m.colB->GetParentBody() == m_body)
            {
                current.emplace(m.colA->GetParentBody(), m);
            }
        }

        for (auto& [rb, cm] : current)
        {
            if (m_collidedBodies.insert(rb).second)
            {
                OnContactEnter(rb, cm);
            }
            else
            {
                OnContactPersist(rb, cm);
            }
        }

        std::vector<RigidBody*> exitingBodies;
        exitingBodies.reserve(m_collidedBodies.size());

        for (RigidBody* prev : m_collidedBodies)
        {
            if (current.find(prev) == current.end())
            {
                exitingBodies.push_back(prev);
            }
        }
        for (RigidBody* rb : exitingBodies)
        {
            OnContactExit(rb);
            m_collidedBodies.erase(rb);
        }
    }
}
