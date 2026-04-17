#include "AudioPlayer.h"

AudioBuffer::AudioBuffer() : size(102400), cache(new int8_t[size])
{
    alGenBuffers(1, &buffer);
}

AudioBuffer::~AudioBuffer()
{
    alDeleteBuffers(1, &buffer);
    delete[] cache;
}

void AudioBuffer::SetTrackDataBlock(std::vector<TrackDataBlock>::iterator blockIt)
{
    this->blockIt = blockIt;
    memcpy(cache, blockIt->ptr, blockIt->byteSize);
    Wave* track = blockIt->wav;
    switch (track->numChannels)
    {
    case 1:
        switch (track->bitsPerSample)
        {
        case 8:
            alBufferData(buffer, AL_FORMAT_MONO8, cache, blockIt->byteSize, blockIt->wav->sampleRate);
            break;

        case 16:
            alBufferData(buffer, AL_FORMAT_MONO16, cache, blockIt->byteSize, blockIt->wav->sampleRate);
            break;
        }
        break;
    case 2:
        switch (track->bitsPerSample)
        {
        case 8:
            alBufferData(buffer, AL_FORMAT_STEREO8, cache, blockIt->byteSize, blockIt->wav->sampleRate);
            break;

        case 16:
            alBufferData(buffer, AL_FORMAT_STEREO16, cache, blockIt->byteSize, blockIt->wav->sampleRate);
            break;
        }
        break;
    }
    
}

AudioBufferPool::AudioBufferPool(int size)
{
    for (int i = 0; i < size; i++)
    {
        AudioBuffer* buffer = new AudioBuffer;
        buffers.push_back(buffer);
		//printf("Create buffer %d\n", buffer->buffer);
    }
}

AudioBuffer* AudioBufferPool::GetBuffer(std::vector<TrackDataBlock>::iterator blockIt)
{
    auto it = std::find_if(buffers.begin(), buffers.end(), [blockIt](AudioBuffer* ab) { return ab->blockIt == blockIt; });
    if (it != buffers.end())
    {
        buffers.splice(buffers.end(), buffers, it);
    }
    else
    {
        it = buffers.begin();
        buffers.splice(buffers.end(), buffers, it);
        (*it)->SetTrackDataBlock(blockIt);
        //blockIt->GenerateSpecturm(GetSpectrumExtractor());
    }
    return *it;
}

AudioTrack* AudioTrackList::AddTrack(Wave* track)
{    
    tracks.push_back(AudioTrack());
    auto t = &tracks.back();
    t->source = track;
    t->id = tracks.size();
    t->trackList = this;
    return t;
}

std::list<AudioTrack>::iterator AudioTrackList::FindTrackIt(AudioTrack* trackPtr)
{
    return std::find_if(tracks.begin(), tracks.end(), [trackPtr](AudioTrack& track) { return &track == trackPtr; });
}

AudioTrack* AudioTrackList::FindTrackByID(int id)
{
    auto it = std::find_if(tracks.begin(), tracks.end(), [id](AudioTrack& track) { return track.id == id; });
    return &*it;
}

AudioTrack* AudioTrackList::GetNextTrack(AudioTrack* track, bool loop)
{
    printf("Current: %s\n", track->source->name.c_str());
    auto it = FindTrackIt(track);
    if (it == tracks.end())
    {
        return nullptr;
    }
    it = std::next(it);
    if (it == tracks.end())
    {
        if (loop)
        {
            it = std::next(it);
        }
        else
        {
            return nullptr;
        }
    }
    printf("Next: %s\n", it->source->name.c_str());
    return &*it;
}

AudioTrack* AudioTrackList::GetPreviousTrack(AudioTrack* track, bool loop)
{
    return nullptr;
}

void AudioTrackList::SwapTrack(int aID, int bID)
{
    auto aIt = FindTrackIt(FindTrackByID(aID));
    auto bIt = FindTrackIt(FindTrackByID(bID));
    std::iter_swap(aIt, bIt);
}

AudioSource::~AudioSource()
{
    alSourceStop(sid);
    if (playingBuffer)
    {
        alSourceUnqueueBuffers(sid, 1, &playingBuffer->buffer);
        playingBuffer = nullptr;
    }
    if (cacheBuffer)
    {
        alSourceUnqueueBuffers(sid, 1, &cacheBuffer->buffer);
        cacheBuffer = nullptr;
    }
    alDeleteSources(1, &sid);
}

void AudioSource::InitializeSource(ALuint sid)
{
    this->sid = sid;

    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.0f;

    direction[0] = 0.0f;
    direction[1] = 0.0f;
    direction[2] = 1.0f;

    velocity[0] = 0.0f;
    velocity[1] = 0.0f;
    velocity[2] = 0.0f;

    volume = 0.5f;

    //alSourcei(sid, AL_SOURCE_RELATIVE, 1);
    alSourcef(sid, AL_CONE_INNER_ANGLE, 2.0f);
    alSourcef(sid, AL_CONE_OUTER_ANGLE, 180.0f);
    alSourcei(sid, AL_SOURCE_TYPE, AL_STATIC);
    alSourcef(sid, AL_PITCH, 1.0f);
    alSourcef(sid, AL_GAIN, volume);
    alSourcefv(sid, AL_POSITION, position);
    alSourcefv(sid, AL_DIRECTION, direction);
    alSourcefv(sid, AL_VELOCITY, velocity);
    alSourcei(sid, AL_LOOPING, false);

	loopType = LoopType::ListLoop;

    alGetSourcei(sid, AL_SOURCE_STATE, &state);
}

void AudioSource::SetSpectrumExtractor(SpectrumExtractor* extractor)
{
    spectrumExtractor = extractor;
}

void AudioSource::Play(AudioTrack* track)
{
    if (track == nullptr)
    {
        return;
    }
    this->track = track;
    Wave* trackSource = track->source;
    if (GetIsPlaying())
    {
        Stop();
        printf("Source:%d Switch track(%s)\n", sid, trackSource->name.c_str());
    }

    playingBuffer = bufferPool->GetBuffer(trackSource->blocks.begin());
    playingBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
    alSourceQueueBuffers(sid, 1, &playingBuffer->buffer);

    cacheBuffer = bufferPool->GetBuffer(trackSource->blocks.begin() + 1);
    cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
    alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);

    alSourcePlay(sid);
    printf("Source %d Play (%s)\n", sid, trackSource->name.c_str());
}

void AudioSource::Stop()
{
    if (GetIsStoppped())
    {
        //printf("Source:%d Already stopped\n", sid);
        return;
	}
    alSourceStop(sid);
    if (playingBuffer)
    {
        alSourceUnqueueBuffers(sid, 1, &playingBuffer->buffer);
        playingBuffer = nullptr;
    }
    if (cacheBuffer)
    {
        alSourceUnqueueBuffers(sid, 1, &cacheBuffer->buffer);
        cacheBuffer = nullptr;
    }
    printf("Source:%d Stop\n", sid);
}

void AudioSource::Pause()
{    
    if (GetIsPlaying())
    {
        alSourcePause(sid);
    }
}

void AudioSource::Resume()
{
    if (GetIsPaused())
    {

        alSourcePlay(sid);
    }
}

void AudioSource::PlayNext()
{

}

void AudioSource::PlayPrevious()
{

}

void AudioSource::QueueBuffer()
{
    alSourceUnqueueBuffers(sid, 1, &playingBuffer->buffer);
    playingBuffer = cacheBuffer;

    if (playingBuffer->blockIt + 1 == GetPlayingTrackSource()->blocks.end())
    {
        AudioTrack* nextTrack = nullptr;
        switch (loopType)
        {
        case LoopType::TrackOnce:
			break;
        case LoopType::TrackLoop:
            cacheBuffer = bufferPool->GetBuffer(GetPlayingTrackSource()->blocks.begin());
            cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
            alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);
            break;
        case LoopType::ListOnce:
            nextTrack = track->trackList->GetNextTrack(track, false);
            if (!nextTrack)
            {
                return;
            }
            cacheBuffer = bufferPool->GetBuffer(nextTrack->source->blocks.begin());
            cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
            alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);
            track = nextTrack;
            break;
        case LoopType::ListLoop:     
            nextTrack = track->trackList->GetNextTrack(track, true);
            if (!nextTrack)
            {
                nextTrack = &*(track->trackList->tracks.begin());
            }
            cacheBuffer = bufferPool->GetBuffer(nextTrack->source->blocks.begin());
            cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
            alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);
            track = nextTrack;
            break;
        }
    }    
    else
    {
        cacheBuffer = bufferPool->GetBuffer(playingBuffer->blockIt + 1);
        cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
        alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);
    }
    
    //printf("Source %d Queue buffer %s:%lu\n", sid, cacheBuffer->sampleBlockIt->wav->name.c_str(), cacheBuffer->sampleBlockIt->ptr);
}

void AudioSource::UpdateVolume()
{
    alSourcef(sid, AL_GAIN, volume);
    //printf("Source %d Update volume to %f\n", sid, volume);
}

void AudioSource::UpdatePosition()
{
    alSourcefv(sid, AL_POSITION, position);
    //printf("Source %d Update position to (%f, %f, %f)\n", sid, position[0], position[1], position[2]);
}

void AudioSource::UpdateDirection()
{
    alSourcefv(sid, AL_DIRECTION, direction);
    //printf("Source %d Update direction to (%f, %f, %f)\n", sid, direction[0], direction[1], direction[2]);
}

void AudioSource::UpdateVelocity()
{
    alSourcefv(sid, AL_VELOCITY, velocity);
    //printf("Source %d Update velocity to (%f, %f, %f)\n", sid, velocity[0], velocity[1], velocity[2]);
}

void AudioSource::SetVolume(float volume)
{
    this->volume = volume;
}

void AudioSource::SetPosition(Vector3F position)
{
    memcpy(this->position, &position, sizeof(float) * 3);
}

void AudioSource::SetDirection(Vector3F direction)
{
    memcpy(this->direction, &direction, sizeof(float) * 3);
}

void AudioSource::SetVelocity(Vector3F velocity)
{
    memcpy(this->velocity, &velocity, sizeof(float) * 3);
}

bool AudioSource::GetIsPlaying()
{
    return state == AL_PLAYING;
}

bool AudioSource::GetIsStoppped()
{
	return state == AL_STOPPED || state == AL_INITIAL;
}

bool AudioSource::GetIsPaused()
{
	return state == AL_PAUSED;
}

Wave* AudioSource::GetPlayingTrackSource()
{
    //return playingBuffer ? playingBuffer->blockIt->wav : nullptr;
    return GetIsStoppped() ? nullptr : playingBuffer->blockIt->wav;
}

float AudioSource::GetPlayingTrackProcessedDuration()
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }

    ALint offset;
    alGetSourcei(sid, AL_BYTE_OFFSET, &offset);
    return (float)(offset + playingBuffer->blockIt->offset) / GetPlayingTrackSource()->byteRate;
}

float AudioSource::GetPlayingTrackTotalDuration()
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }

    return GetPlayingTrackSource()->duration;
}

float AudioSource::GetProgress()
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }

    ALint offset;
    alGetSourcei(sid, AL_BYTE_OFFSET, &offset);
    return (float)(offset + playingBuffer->blockIt->offset) / GetPlayingTrackSource()->byteSize;
}

void AudioSource::SetProgress(float progress)
{
    if (GetIsStoppped())
    {
        return;
    }   
    Wave* track = GetPlayingTrackSource();
	int offset = progress * track->byteSize;
    int idx = offset / track->blockSize;
	offset = offset % track->blockSize;
	auto it = track->blocks.begin() + idx;
    if (state == AL_PLAYING)
    {
        Stop();        
    }

    playingBuffer = bufferPool->GetBuffer(it);
    playingBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
    alSourceQueueBuffers(sid, 1, &playingBuffer->buffer);

    cacheBuffer = bufferPool->GetBuffer(it + 1);
    cacheBuffer->blockIt->GenerateSpecturm(spectrumExtractor);
    alSourceQueueBuffers(sid, 1, &cacheBuffer->buffer);

	alSourcei(sid, AL_BYTE_OFFSET, offset);

    alSourcePlay(sid);
    printf("Source %d Set progress to %f(%fs)\n", sid, progress, progress * track->duration);
}

float AudioSource::GetWaveformMagnitude()
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }

    int sampleOffset;
    alGetSourcei(sid, AL_SAMPLE_OFFSET, &sampleOffset);

    int8_t* left = playingBuffer->blockIt->ptr + sampleOffset * (playingBuffer->blockIt->wav->bitsPerSample / 8) * playingBuffer->blockIt->wav->numChannels;
    int8_t* right = playingBuffer->blockIt->ptr + sampleOffset * (playingBuffer->blockIt->wav->bitsPerSample / 8) * playingBuffer->blockIt->wav->numChannels + 2;
    int16_t magnitude1 = (*(int16_t*)left + *(int16_t*)right) / 2;
    return magnitude1;
}

Spectrum* AudioSource::GetSpectrum()
{
    if (GetIsStoppped())
    {
        return nullptr;
    }

    int sampleOffset;
    alGetSourcei(sid, AL_SAMPLE_OFFSET, &sampleOffset);
    int spectrumIdx = sampleOffset / (FT_WINDOW_SIZE / 2);

    return &playingBuffer->blockIt->spectrum[spectrumIdx];
}

float AudioSource::GetSpectrumMagnitude(int idx)
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }
    //printf("size: %d\n", playingBuffer->blockIt->spectrum.size());
    int sampleOffset;
    alGetSourcei(sid, AL_SAMPLE_OFFSET, &sampleOffset);
    int spectrumIdx = sampleOffset / (FT_WINDOW_SIZE / 2);
    int frequencyIdx = idx;
    //printf("spectrumIdx: %d, frequencyIdx: %d\n", spectrumIdx, frequencyIdx);
    double magnitude1 = (float)playingBuffer->blockIt->spectrum[spectrumIdx].magnitudes[frequencyIdx];
    if (spectrumIdx == playingBuffer->blockIt->spectrum.size() - 1)
    {
        return 20 * log10(magnitude1);
    }
    double magnitude2 = (float)playingBuffer->blockIt->spectrum[spectrumIdx + 1].magnitudes[frequencyIdx];
    double offset = (sampleOffset % (FT_WINDOW_SIZE / 2)) / (double)(FT_WINDOW_SIZE / 2);
    return 20 * log10(magnitude1 * (1 - offset) + magnitude2 * offset);
}

float AudioSource::GetSpectrumMagnitude(double frequency)
{
    if (GetIsStoppped())
    {
        return 0.0f;
    }
    
    int sampleOffset;
    alGetSourcei(sid, AL_SAMPLE_OFFSET, &sampleOffset);
    int spectrumIdx = sampleOffset / (FT_WINDOW_SIZE / 2);
    int frequencyIdx = frequency / (GetPlayingTrackSource()->sampleRate / 2) * (FT_WINDOW_SIZE / 2);
    //printf("spectrumIdx: %d, frequencyIdx: %d\n", spectrumIdx, frequencyIdx);
    double magnitude1 = (float)playingBuffer->blockIt->spectrum[spectrumIdx].magnitudes[frequencyIdx];
    if (spectrumIdx == playingBuffer->blockIt->spectrum.size() - 1)
    {
        return 20 * log10(magnitude1);
    }
    double magnitude2 = (float)playingBuffer->blockIt->spectrum[spectrumIdx + 1].magnitudes[frequencyIdx];
    double offset = (sampleOffset % (FT_WINDOW_SIZE / 2)) / (double)(FT_WINDOW_SIZE / 2);
    return 20 * log10(magnitude1 * (1 - offset) + magnitude2 * offset);
}

void AudioSource::Tick(float deltaTime)
{
    alGetSourcei(sid, AL_SOURCE_STATE, &state);    

    if (GetIsStoppped())
    {        
        return;
    }
    
    ALint processed;
    alGetSourcei(sid, AL_BUFFERS_PROCESSED, &processed);
    if (processed == 1)
    {
        QueueBuffer();
    }

}

void AudioPlayer::InitializeOpenAL()
{
    /*
    std::vector<std::string> devices;
    const ALCchar* device_list = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    while (strlen(device_list) > 0) 
    {
        devices.push_back(device_list);
        device_list += strlen(device_list) + 1;
    }
    for (auto& device : devices)
    {
        std::cout << device << std::endl;
    }
    */
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        printf("Failed to open OpenAL device\n");
    }
    printf("Device: %s\n", alcGetString(device, ALC_ALL_DEVICES_SPECIFIER));

    context = alcCreateContext(device, nullptr);
    if (!context)
    {
        printf("Failed to create OpenAL context\n");
    }

    alcMakeContextCurrent(context);

    bufferPool = new AudioBufferPool(20);

    spectrumExtractor = new SpectrumExtractor(44100);

    looping = false;

    listenerPosition[0] = 0.0f;
    listenerPosition[1] = 0.0f;
    listenerPosition[2] = 0.0f;

    listenerOrientation[0] = 0.0f;
    listenerOrientation[1] = 0.0f;
    listenerOrientation[2] = -1.0f;
    listenerOrientation[3] = 0.0f;
    listenerOrientation[4] = 1.0f;
    listenerOrientation[5] = 0.0f;

    listenerVelocity[0] = 0.0f;
    listenerVelocity[1] = 0.0f;
    listenerVelocity[2] = 0.0f;

    alListener3f(AL_POSITION, listenerPosition[0], listenerPosition[1], listenerPosition[2]);
    alListenerfv(AL_ORIENTATION, listenerOrientation);
    alListener3f(AL_VELOCITY, listenerVelocity[0], listenerVelocity[1], listenerVelocity[2]);

    timeCounter = 0.0f;
}

void AudioPlayer::CloseOpenAL()
{
    for (auto& source : sources)
    {
        delete source;
	}
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

AudioPlayer::AudioPlayer()
{
    InitializeOpenAL();
}

AudioPlayer::~AudioPlayer()
{
    CloseOpenAL();
}

AudioSource* AudioPlayer::CreateSource()
{
    if (sources.size() >= 10)
    {
        printf("Max sources size = 10\n");
        return nullptr;
    }
    ALuint* sids = new ALuint;
    alGenSources(1, sids);
    AudioSource* source = new AudioSource();
    source->player = this;
    source->bufferPool = bufferPool;
    source->InitializeSource(*sids);
    source->SetSpectrumExtractor(spectrumExtractor);
    delete sids;
    sources.push_back(source);
    printf("Create source%d\n", source->sid);
    return source;
}

void AudioPlayer::DestorySource(AudioSource* source)
{
    for (auto it = sources.begin(); it != sources.end(); it++)
    {
        if (*it == source)
        {
            source->Stop();
            int sid = source->sid;
            delete source;
            sources.erase(it);
            printf("Destory source%d\n", sid);
            break;
        }
    }
}

AudioTrackList* AudioPlayer::CreateTrackList(const char* name)
{
    trackList.push_back(AudioTrackList());

    auto it = std::prev(trackList.end());
    /*
    auto& window = *it;
    window.id = trackList.size();
    window.name.assign(name);
    window.nameU8 = GbkToUtf8(window.name);    
    */
    it->id = trackList.size();
    it->name.assign(name);
    it->nameU8 = GbkToUtf8(it->name);
    return &*it;
}

std::list<AudioTrackList>::iterator AudioPlayer::FindTrackListIt(AudioTrackList* trackListPtr)
{
    return std::find_if(trackList.begin(), trackList.end(), [trackListPtr](AudioTrackList& list) { return &list == trackListPtr; });
}

AudioTrackList* AudioPlayer::GetNextTrackList(AudioTrackList* trackListPtr)
{
    auto it = FindTrackListIt(trackListPtr);
    it = std::next(it);
    return &*it;
}

void AudioPlayer::UpdateListenerPosition()
{    
    alListener3f(AL_POSITION, listenerPosition[0], listenerPosition[1], listenerPosition[2]);
    //printf("Set listener position to (%f, %f, %f)\n", listenerPosition[0], listenerPosition[1], listenerPosition[2]);
}

void AudioPlayer::UpdateListenerRotation()
{    
    alListenerfv(AL_ORIENTATION, listenerOrientation);
    //printf("Set listener direction to (%f, %f, %f), (%f, %f, %f)\n", listenerOrientation[0], listenerOrientation[1], listenerOrientation[2], listenerOrientation[3], listenerOrientation[4], listenerOrientation[5]);
}

void AudioPlayer::UpdateListenerVelocity()
{
    alListener3f(AL_VELOCITY, listenerVelocity[0], listenerVelocity[1], listenerVelocity[2]);
    //printf("Set listener velocity to (%f, %f, %f)\n", listenerVelocity[0], listenerVelocity[1], listenerVelocity[2]);
}

void AudioPlayer::SetListenerPosition(Vector3F position)
{
    memcpy(this->listenerPosition, &position, sizeof(float) * 3);
}

void AudioPlayer::SetListenerRotation(Rotator rotation)
{
    memcpy(listenerRotation, &rotation, sizeof(float) * 3);
    Vector3F forward = rotation.Rotate(Vector3F(0, 0, -1));
    Vector3F up = rotation.Rotate(Vector3F(0, 1, 0));
    memcpy(listenerOrientation, &forward, sizeof(float) * 3);
    memcpy(listenerOrientation + 3, &up, sizeof(float) * 3);
    //printf("Set listener rotation: %f, %f, %f(f: %f, %f, %f, u: %f, %f, %f)\n", rotation.yaw, rotation.pitch, rotation.roll, forward.x, forward.y, forward.z, up.x, up.y, up.z);
}

void AudioPlayer::SetListenerVelocity(Vector3F velocity)
{
    memcpy(this->listenerVelocity, &velocity, sizeof(float) * 3);
}

float AudioPlayer::GetSpectrumMagnitude(double frequency)
{
    return 1.0f;
}

void AudioPlayer::Tick(float deltaTime)
{
	timeCounter += deltaTime;
    if (timeCounter >= 0.2f)
    {
        timeCounter = 0.0f;
        UpdateListenerPosition();
        UpdateListenerRotation();
        UpdateListenerVelocity();
        for (auto& source : sources)
        {
            source->UpdatePosition();
            source->UpdateDirection();
            source->UpdateVelocity();
            source->Tick(deltaTime);
        }
	}
    else
    {
        for (auto& source : sources)
        {
            source->Tick(deltaTime);
        }
    }
}