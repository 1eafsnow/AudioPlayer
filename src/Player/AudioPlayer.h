#pragma once

#include <cstdio>
#include <random>
#include <al.h>
#include <alc.h>
#include <list>
#include <functional>
#include "../Math/MathS.h"
#include "../Data/Wave.h"
#include "../Data/Spectrum.h"

enum class LoopType
{
    TrackOnce = 0,
    TrackLoop = 1,
	ListOnce = 2,
    ListLoop = 3,
    ListRandom = 4
};

class AudioBuffer
{
public:
    int size;
    int8_t* cache;
    ALuint buffer;
    std::vector<TrackDataBlock>::iterator blockIt;

    AudioBuffer();
    ~AudioBuffer();
    void SetTrackDataBlock(std::vector<TrackDataBlock>::iterator blockIt);
};

class AudioBufferPool
{
public:
    std::list<AudioBuffer*> buffers;

    AudioBufferPool(int size = 20);
    AudioBuffer* GetBuffer(std::vector<TrackDataBlock>::iterator blockIt);
};

class AudioTrackList;
class AudioTrack
{
public:
    int id;
    Wave* source;
    AudioTrackList* trackList;
};

class AudioTrackList
{
public:
    int id;
    std::string name;
    std::string nameU8;
    std::list<AudioTrack> tracks;

    AudioTrack* AddTrack(Wave* track);
    std::list<AudioTrack>::iterator FindTrackIt(AudioTrack* trackPtr);
    AudioTrack* FindTrackByID(int id);
    AudioTrack* GetNextTrack(AudioTrack* track, bool loop = true);
    AudioTrack* GetPreviousTrack(AudioTrack* track, bool loop = true);
    void SwapTrack(int aID, int bID);
};

struct AudioBufferFinderByBuffer
{
    AudioBufferFinderByBuffer(ALuint buffer) : buffer(buffer) {} bool operator()(AudioBuffer* ab) { return (buffer == ab->buffer); } ALuint buffer;
};

class AudioPlayer;
class AudioSource
{
public:
    ALuint sid;
    ALfloat position[3];
    ALfloat direction[3];
    ALfloat velocity[3];
    float volume;
    ALint state;

    AudioPlayer* player;

    AudioTrack* track;
    SpectrumExtractor* spectrumExtractor;

    AudioBufferPool* bufferPool;
    AudioBuffer* playingBuffer;
    AudioBuffer* cacheBuffer;    

	LoopType loopType;

    ~AudioSource();
    void InitializeSource(ALuint sid);
    void SetSpectrumExtractor(SpectrumExtractor* extractor);
    void Play(AudioTrack* track);
    void Stop();
	void Pause();
	void Resume();
    void PlayNext();
    void PlayPrevious();
    void QueueBuffer();
    void UpdateVolume();
    void UpdatePosition();
    void UpdateDirection();
    void UpdateVelocity();
    void SetVolume(float volume);
    void SetPosition(Vector3F position);
    void SetDirection(Vector3F direction);
    void SetVelocity(Vector3F velocity);
    bool GetIsPlaying();
    bool GetIsStoppped();
    bool GetIsPaused();
    Wave* GetPlayingTrackSource();
    float GetPlayingTrackProcessedDuration();
    float GetPlayingTrackTotalDuration();
    float GetProgress();
    void SetProgress(float progress);
    float GetWaveformMagnitude();
    Spectrum* GetSpectrum();
    float GetSpectrumMagnitude(int idx);
    float GetSpectrumMagnitude(double frequency);

	void Tick(float deltaTime);
};

class AudioPlayer
{
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
	//int sourceCount;
    ALint looping;
    std::vector<AudioSource*> sources;

    AudioBufferPool* bufferPool;

    SpectrumExtractor* spectrumExtractor;
	Spectrum spectrum;

    void InitializeOpenAL();
    void CloseOpenAL();

    void UpdateListenerPosition();
    void UpdateListenerRotation();
    void UpdateListenerVelocity();

public:
    ALfloat listenerPosition[3];
    ALfloat listenerOrientation[6];
    ALfloat listenerVelocity[3];
    ALfloat listenerRotation[3];

    std::list<AudioTrackList> trackList;

    float timeCounter;

    AudioPlayer();
    ~AudioPlayer();
    AudioSource* CreateSource();
    void DestorySource(AudioSource* source);
    AudioTrackList* CreateTrackList(const char* name);
    std::list<AudioTrackList>::iterator FindTrackListIt(AudioTrackList* trackListPtr);
    AudioTrackList* GetNextTrackList(AudioTrackList* trackListPtr);
    void SetListenerPosition(Vector3F position);
    void SetListenerRotation(Rotator rotation);
    void SetListenerVelocity(Vector3F velocity);
    float GetSpectrumMagnitude(double frequency);
    void Tick(float deltaTime);
};