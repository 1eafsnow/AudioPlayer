#pragma once

#include <stdint.h>
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>
#include <codecvt>
#include <Windows.h>
#include "Spectrum.h"

#define WAVE_ID uint32_t	

#define MAKE_WAVE_ID(a,b,c,d) \
(((uint32_t)d) | ( ((uint32_t)c) << 8 ) | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ))

template <char ch0, char ch1, char ch2, char ch3> struct MakeWaveID { enum { value = (ch0 << 0) + (ch1 << 8) + (ch2 << 16) + (ch3 << 24) }; };

struct ChunkRIFF
{
	WAVE_ID chunkID;
	uint32_t chunkSize;
	WAVE_ID format;
};

struct ChunkFormat
{
	WAVE_ID chunkID;
	uint32_t chunkSize;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct Chunk
{
	WAVE_ID chunkID;
	uint32_t chunkSize;
};

struct Header
{
	ChunkRIFF* riff;
	ChunkFormat* format;
	std::vector<Chunk*> chunks;

	Header() : riff(nullptr), format(nullptr) {}

	Header(const Header& header) : riff(new ChunkRIFF()), format(new ChunkFormat())
	{		
		memcpy(riff, header.riff, sizeof(ChunkRIFF));
		memcpy(format, header.format, sizeof(ChunkFormat));
		for (const auto& chunk : header.chunks)
		{
			Chunk* newChunk = new Chunk();
			memcpy(newChunk, chunk, sizeof(Chunk));
			chunks.push_back(newChunk);
		}
	}

	void Print()
	{
		char id[4];
		memcpy(id, &riff->chunkID, 4);
		printf("SChunkID: %c%c%c%c\n", id[0], id[1], id[2], id[3]);
		printf("ChunkSize: %u\n", riff->chunkSize);
		memcpy(id, &riff->format, 4);
		printf("Format: %c%c%c%c\n", id[0], id[1], id[2], id[3]);
		memcpy(id, &format->chunkID, 4);
		printf("FmtChunkID: %c%c%c%c\n", id[0], id[1], id[2], id[3]);
		printf("FmtChunkSize: %u\n", format->chunkSize);
		printf("AudioFormat: %d\n", format->audioFormat);
		printf("NumChannels: %d\n", format->numChannels);
		printf("SampleRate: %u\n", format->sampleRate);
		printf("ByteRate: %u\n", format->byteRate);
		printf("BlockAlign: %d\n", format->blockAlign);
		printf("BitsPerSample: %d\n", format->bitsPerSample);

		for (auto& chunk : chunks)
		{
			memcpy(id, &chunk->chunkID, 4);
			printf("ChunkID: %c%c%c%c\n", id[0], id[1], id[2], id[3]);
			printf("ChunkSize: %u\n", chunk->chunkSize);
		}
	}
};

class TrackDataBlock
{
public:
	Wave* wav;
	int offset;
	int byteSize;
	int sampleSize;
	int8_t* ptr;
	std::vector<Spectrum> spectrum;

	void GenerateSpecturm(SpectrumExtractor* extractor);
};

class Wave
{
private:

public:
	std::string path;
	std::string pathU8;
	std::string name;
	std::string nameU8;
	Header* header;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint32_t sampleSize;
	uint16_t bitsPerSample;
	uint32_t byteSize;
	float duration;
	int8_t* data;	
	int blockSize;
	std::vector<TrackDataBlock> blocks;
	void PrintInfo();
	void Load(const char* path);
	void GenerateBlocks();
};

std::string WinUtf8ToGbk(std::string utf8Str);
std::string WinGbkToUtf8(std::string gbkStr);
std::string GbkToUtf8(std::string& str);
std::string PathToName(std::string path);
void StereoToMono(Wave& stereo, Wave& mono);
void Filter(Wave& track);