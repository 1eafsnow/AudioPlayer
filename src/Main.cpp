#include <Windows.h>
#include <stdlib.h>
#include "Player/AudioPlayer.h"
#include "GUI//GUI.h"
#include "Common/Time.h"

int main(int argc, char** argv)
{
	AudioPlayer player;
	SpectrumExtractor spectrumExtractor(44100);
	//spectrumExtractor.Extract();

	GUI gui;
	gui.audioPlayer = &player;
	gui.Open();

	gui.CreateAudioSource(Vector3F(0.0, 0.0, -5.0));

	gui.CreateTrackList("TrackList1");
	gui.CreateTrackList("TrackList2");
	gui.AddTrack(1, "C:\\FFOutput\\ÁõÏ§¾ý - Á÷¹â·ÉÎè (¹úÓï)_EM.wav");
	gui.AddTrack(1, "C:\\FFOutput\\±õÆé²½ - Who____EM.wav");
	gui.AddTrack(1, "C:\\FFOutput\\X JAPAN - Tears¡«'93 TOKYO DOME LIVE VERSION¡« (Live).wav");
	
	//Spectrogram spectrogram;
	//spectrogram.sampleRate = 44100;
	//spectrumExtractor.Extract(&wav31, &spectrogram); 

	//gui.spectrogram = &spectrogram;

	int64_t time = GetTime();

	while (gui.showMainWindow)
	{
		int64_t deltaTime = GetTime() - time;
		time += deltaTime;
		float deltaSecond = deltaTime / 1000.0;
		//printf("%ld\n", deltaTime);

		player.Tick(deltaSecond);
		gui.Tick(deltaSecond);

	}
	gui.Close();
	return 0;
}