#include "wellness.hpp"

Wellness::Wellness()
	:
	m_updateComponent(Wellness::Component::Sleep)
{
	m_values[Happiness] = 0.8f;
	m_values[Sleep] = 0.8f;
	m_values[Food] = 0.8f;
	m_values[Water] = 0.8f;
	m_values[Cleanliness] = 0.8f;
}

Wellness::Wellness(float defaultValue)
	:
	m_updateComponent(Wellness::Component::Sleep)
{
	m_values[Happiness] = defaultValue;
	m_values[Sleep] = defaultValue;
	m_values[Food] = defaultValue;
	m_values[Water] = defaultValue;
	m_values[Cleanliness] = defaultValue;
}

float Wellness::GetValue(Component component)
{
	return m_values[component];
}

void Wellness::AddValue(Component component, float value)
{
	m_values[component] += value;
}

Wellness::Component Wellness::GetLowest()
{
	Component lowestComponent = Happiness;
	if (m_values[Sleep] < m_values[lowestComponent]) 
	{
		lowestComponent = Sleep;
	}
	if (m_values[Food] < m_values[lowestComponent]) 
	{
		lowestComponent = Food;
	}
	if (m_values[Water] < m_values[lowestComponent]) 
	{
		lowestComponent = Water;
	}
	if (m_values[Cleanliness] < m_values[lowestComponent]) 
	{
		lowestComponent = Cleanliness;
	}
	return lowestComponent;
}

bool Wellness::IsLapsed()
{
	if (m_values[Happiness] <= 0
			|| m_values[Sleep] <= 0
			|| m_values[Food] <= 0
			|| m_values[Water] <= 0
			|| m_values[Cleanliness] <= 0) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void Wellness::Update(float deltaTime)
{
	m_timeSinceUpdate += deltaTime;
	if (m_timeSinceUpdate >= m_secondsBetweenUpdate) 
	{

		// Decrease random wellness.
		m_values[m_updateComponent] += m_updateAmount;

		// Reset state for next time.
		m_updateComponent = (Component) (rand() % 5);

		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = m_updateMax - m_updateMin;
		float r = random * diff;
		m_updateAmount = m_updateMin + r;

		m_timeSinceUpdate -= m_secondsBetweenUpdate;
	}
}
