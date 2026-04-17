#pragma once
#include "../Player/AudioPlayer.h"

class AudioSourceWindow
{
public:
	AudioSource* source = nullptr;
	char label[10] = { 's', 'o', 'u', 'r', 'c', 'e' };
	float progress = 0.0f;
	bool isHovered = false;
};