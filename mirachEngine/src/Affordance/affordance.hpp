#pragma once
#include <Wellness/wellness.hpp>
#include <ECS/ecs_types.hpp>
#include <Math/math.hpp>
#include <unordered_map>

class Affordance 
{
public:
	enum class Action 
	{
		Sleep,
		Eat,
		Play
	};

	Affordance();

	// Return true if entity can interact.
	bool CanInteract(Entity e);

	// Try to do the action. Return false if action cannot be done.
	bool TryInteract(Entity e);

	// Call to signal the action has finished.
	void FinishInteract(Entity e);

	//Get method
	unsigned int GetLimit() const;
	unsigned int GetCount() const;

	void SetAction(const Action& action);

	void SetPosition(const Vector3f& position);

	const Vector3f& GetPosition() const;

	void AddWellnessEffect(const Wellness::Component& component, float effect);

	const std::unordered_map<Wellness::Component, float>& GetWellnessEffects() const;

private:
	std::unordered_map<Wellness::Component, float> m_wellnessEffect;
	Vector3f m_position;
	Action m_action;

	unsigned int m_interactorLimit;
	unsigned int m_interactorCount;
	std::unordered_map<Entity, unsigned int> m_interactors;
};