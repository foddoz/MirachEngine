#pragma once
#include <unordered_map>

class Wellness 
{
public:
	enum Component 
	{
		Happiness,
		Sleep,
		Food,
		Water,
		Cleanliness
	};

	Wellness();
	Wellness(float defaultValue);
	
	float GetValue(Component component);

	void AddValue(Component component, float value);

	// Get component with the lowest value.
	Component GetLowest();

	// Return true if any one component is <= 0.
	bool IsLapsed();

	void Update(float deltaTime);

private:
	float m_secondsBetweenUpdate = 3.f;
	float m_updateMin = -0.005f;
	float m_updateMax = -0.032f;
	float m_timeSinceUpdate = 0.f;
	float m_updateAmount = 0.f;
	Component m_updateComponent;
	std::unordered_map<Component, float> m_values;

};