#include "emotion_system.hpp"

void UpdatePAD(EmotionModel& PAD, EmotionModel& EventPAD, std::string& nature)
{
	//Range between -1 ~ 1
	const NatureWeight nw = Nature::GetNatureWeight(nature);

	//Calculate Pleasure
	float newPleasure = ComputePAD(PAD.pad.pleasure, EventPAD.pad.pleasure, nw.Ppos, nw.Pneg);
	PAD.pad.pleasure = newPleasure;

	//Calculate Arousal
	float newArousal = ComputePAD(PAD.pad.arousal, EventPAD.pad.arousal, nw.Apos, nw.Aneg);
	PAD.pad.arousal = newArousal;

	//Calculate Dominance
	float newDominance = ComputePAD(PAD.pad.dominance, EventPAD.pad.dominance, nw.Dpos, nw.Dneg);
	PAD.pad.dominance = newDominance;
}

float ComputePAD(float current, float delta, float wPos, float wNeg)
{
	//Choose positive or negetive weight
	//new PAD = PAD + (+)delta * Positive Weight | PAD + (-)delta * Negative Weight
	const float weight = (delta >= 0.0f) ? wPos : wNeg;
	float result = current + delta * weight;
	result = std::clamp(result, -1.0f, 1.0f);
	return result;
}

Emotion GetEmotionFromPAD(const PADModel& pad)
{
	const float P = pad.pleasure;
	const float A = pad.arousal;
	const float D = pad.dominance;

	// Just use it for quick test
	//Pleasure (high) ¡ú Happiness
	//Pleasure(low) and Arousal(low) ¡ú Sadness
	//Pleasure(low) and Arousal(high) and Dominance(high) ¡ú Anger
	//Pleasure(low) and Arousal(high) and Dominance(low) ¡ú Fear
	if (P > 0.3f && A > 0.1f && D > -0.2f) 
	{
		return Emotion::Happy;
	}
	else if (P < -0.3f && A < 0.2f) 
	{
		return Emotion::Sad;
	}
	else if (P < 0.0f && A > 0.4f && D > 0.2f)
	{
		return Emotion::Angry;
	}
	else if (P < -0.2f && A > 0.4f && D < -0.3f)
	{
		return Emotion::Scared;
	}

	// Fallback: pick whichever dimension dominates
	if (P >= 0.f) 
	{
		return Emotion::Happy;
	}
	else 
	{
		return Emotion::Sad;
	}
}

std::string GetEmotionString(Emotion emotion) 
{
	switch (emotion) 
	{
	case Emotion::Happy:
		return "Happy";
	case Emotion::Sad:
		return "Sad";
	case Emotion::Angry:
		return "Angry";
	case Emotion::Scared:
		return "Scared";
	default:
		return "Unknown";
	}
}