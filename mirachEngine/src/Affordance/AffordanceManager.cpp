#include "AffordanceManager.hpp"

void AffordanceManager::Register(Entity e, const Affordance& affordance) 
{
    AffordanceRecord record;
    record.data = affordance;
    record.position = affordance.GetPosition();
    record.limit = affordance.GetLimit();
    record.count = affordance.GetCount();
    m_affordanceTable[e] = record;

    for (auto& [wellness, effect] : affordance.GetWellnessEffects()) 
    {
        if (effect != 0.f) 
        {
            m_wellnessTable[wellness].push_back(e);
        }
    }
}

void AffordanceManager::Unregister(Entity e) 
{
    auto it = m_affordanceTable.find(e);
    if (it == m_affordanceTable.end())
    {
        return;
    }
    const auto& affordance = it->second.data;

    // Remove from wellness table
    for (auto& [wellness, _] : affordance.GetWellnessEffects()) 
    {
        auto& vec = m_wellnessTable[wellness];
        vec.erase(std::remove(vec.begin(), vec.end(), e), vec.end());
    }

    m_affordanceTable.erase(it);
}

//Squared distance
//Distance between position a and b
static float sqrDistance(const Vector3f& a, const Vector3f& b) 
{
    Vector3f distance = a - b;
    return distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
}

Entity AffordanceManager::FindNearest(Wellness::Component wellness, const Vector3f& position, float maxDist) const {
    auto it = m_wellnessTable.find(wellness);
    if (it == m_wellnessTable.end())
    {
        return 0; // no affordance for this wellness type
    }

    float bestDistance = maxDist * maxDist;
    Entity nearestEntity = 0;
    for (Entity e : it->second) 
    {
        auto jt = m_affordanceTable.find(e);
        if (jt == m_affordanceTable.end())
        {
            continue; // skip invalid entries
        }

        const AffordanceRecord& record = jt->second;
        if (record.count >= record.limit)
        {
            continue; // skip full/occupied affordances
        }

        // skip full/occupied affordances
        float distance = sqrDistance(position, record.position);
        if (distance < bestDistance) 
        {
            bestDistance = distance;
            nearestEntity = e;
        }
    }
    return nearestEntity;
}

// Checks whether the affordance can be used by the given user,
// based on its usage limit (e.g., 1 user for a bed, 2¨C3 for a food bowl)
bool AffordanceManager::TryReserve(Entity affordanceEntity, Entity User) 
{
    //Find the affordance record
    auto it = m_affordanceTable.find(affordanceEntity);
    {
        if (it == m_affordanceTable.end()) return false; // Not registered affordance
    }
    auto& record = it->second;

    //Already reserved by this user
    if (record.currentUser.count(User)) 
    {
        return true;      
    }
    {
        if (record.count >= record.limit)    //Full: cannot take more users
        {
            return false;
        }
    }
    //Reserve this affordance
    record.currentUser.insert(User);
    ++record.count;
    return true;
}

//Release user from using affordance
void AffordanceManager::Release(Entity affordanceEntity, Entity User) 
{
    auto it = m_affordanceTable.find(affordanceEntity);
    if (it == m_affordanceTable.end())
    {
        return;
    }
    auto& record = it->second;
    //Remove this User from the current user set
    if (record.currentUser.erase(User)) 
    {
        record.count = (record.count > 0) ? record.count - 1 : 0;
    }
}
