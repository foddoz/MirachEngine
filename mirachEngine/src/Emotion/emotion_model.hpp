#pragma once
#include <algorithm>
#include <string>

struct PADModel 
{
	float pleasure = 0.f;
	float arousal = 0.f;
	float dominance = 0.f;
};

enum class Emotion 
{
    Happy, 
    Sad, 
    Angry, 
    Scared
};

struct EmotionModel 
{
    PADModel pad{};
    Emotion  emotion = Emotion::Happy;
};

//Function that make sure PAD value won't out of range(-1 ~ 1)
inline void ClampPAD(PADModel& p) 
{
    auto clamp01 = [](float v) 
    { 
         return std::clamp(v, -1.f, 1.f); 
    };
    p.pleasure = clamp01(p.pleasure);
    p.arousal = clamp01(p.arousal);
    p.dominance = clamp01(p.dominance);
}