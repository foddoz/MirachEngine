#include "affordance.hpp"

Affordance::Affordance()
	:
	m_interactorLimit(1),
	m_interactorCount(0),
	m_position(Vector3f(0.f)),
	m_action(Action::Sleep)
{

}

bool Affordance::CanInteract(Entity e)
{
	return m_interactorCount < m_interactorLimit;
}

bool Affordance::TryInteract(Entity e)
{
	if (CanInteract(e)) 
	{
		m_interactorCount += 1;
		m_interactors[e] += 1;
		return true;
	}
	return false;
}

void Affordance::FinishInteract(Entity e)
{
	if (m_interactors[e] > 0) 
	{
		m_interactors[e] -= 1;
		m_interactorCount -= 1;
	}
}

unsigned int Affordance::GetCount() const
{
	return m_interactorCount;
}

unsigned int Affordance::GetLimit() const
{
	return m_interactorLimit;
}

void Affordance::SetAction(const Action& action)
{
	m_action = action;
}

void Affordance::SetPosition(const Vector3f& position)
{
	m_position = position;
}

const Vector3f& Affordance::GetPosition() const
{
	return m_position;
}

void Affordance::AddWellnessEffect(const Wellness::Component& component, float effect)
{
	m_wellnessEffect[component] = effect;
}

const std::unordered_map<Wellness::Component, float>& Affordance::GetWellnessEffects() const
{
	return m_wellnessEffect;
}