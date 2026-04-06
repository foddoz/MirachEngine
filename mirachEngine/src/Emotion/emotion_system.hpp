#pragma once
#include "emotion_model.hpp"
#include "nature.hpp"


void UpdatePAD(EmotionModel& PAD, EmotionModel& EventPAD, std::string& nature);

float ComputePAD(float current, float delta, float wPos, float wNeg);

Emotion GetEmotionFromPAD(const PADModel& pad);

std::string GetEmotionString(Emotion emotion);