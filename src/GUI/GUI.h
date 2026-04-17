#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>
#include <vector>
#include <map>
//#include <GL/glew.h>
//#define GL_SILENCE_DEPRECATION
//#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "../Player/AudioPlayer.h"
#include "../Data/Spectrum.h"
#include "../Render/Renderer.h"

class AudioSourceWindow
{
public:
	int id = -1;
	AudioSource* source = nullptr;
	char label[10] = { 's', 'o', 'u', 'r', 'c', 'e' };
	float progress = 0.0f;
	bool isHovered = false;
	Sphere* mesh = nullptr;
	int windowFlags = 0;
	bool show = true;

	void Render();
};

class TrackItem
{
public:
	int id = -1;
	AudioTrack* track;
	bool isSelected = false;

	void Render();
};

class TrackListWindow
{
public:
	int id = -1;
	bool show = true;
	AudioTrackList* trackListIt;
	std::vector<TrackItem> trackItems;
	int windowFlags = 0;
	bool isHovered = false;

	void Render();
};

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

class Input;

class GUI
{
public:
	int width = 1600;
	int height = 900;

	GLFWwindow* glWindow;
	//ImGuiIO* io;

	AudioPlayer* audioPlayer;
	std::vector<AudioSourceWindow> audioSourceWindows;
	std::vector<TrackListWindow> trackListWindows;

	Spectrogram* spectrogram;

	Renderer* renderer;
	Input* input;

	ImGuiID dockspaceID; 
	ImGuiID leftNode;
	ImGuiID rightNode;
	ImGuiID leftUpNode;
	ImGuiID leftDownNode;

	bool showMainWindow;
	bool showListener;
	
	GUI();
	~GUI();

	void DrawPixels(unsigned char* pixels);
	void CreateAudioSource(Vector3F location = Vector3F(0.0, 0.0, 0.0));
	void DestoryAudioSource(int sourceId);
	void CreateTrackList(const char* name);
	void AddTrack(int trackListID, const char* path);
	void AddTrack(int trackListID, TrackItem* track);

	void ShowAudioSourceWindow();
	void ShowListenerWindow();
	void ShowTrackListWindow();
	void ShowTrackItem(TrackListWindow& trackListWindow, TrackItem& trackItem);

	void Open();
	void Close();
	void Tick(float deltaTime);
};