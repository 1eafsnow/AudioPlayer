#pragma once
#include <stdint.h>

class GUI;

class Input
{
public:
	enum State : uint32_t
	{
		NUM1 = (1 << 0),
		NUM2 = (1 << 1),
		ENTER = (1 << 2),
		TAB = (1 << 3),
		MOUSE_LEFT = (1 << 4),
		MOUSE_RIGHT = (1 << 5)
	};

private:
	GUI* gui;
	uint32_t state;
	float mouseX;
	float mouseY;

public:
	void Press(uint32_t input);
	void Release(uint32_t input);
	bool Check(uint32_t input);
	void SetGUI(GUI* gui);
	void Tick(float deltaTime);
};