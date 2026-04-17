#include "Wave.h"

void TrackDataBlock:: GenerateSpecturm(SpectrumExtractor* extractor)
{
	spectrum.resize(sampleSize / (FT_WINDOW_SIZE / 2));
	//printf("sampleSize:%d, spec size: %d\n", sampleSize, spectrum.size());
	for (int i = 0; i < spectrum.size(); i++)
	{
		extractor->Extract(ptr + i * (FT_WINDOW_SIZE / 2) * wav->numChannels * (wav->bitsPerSample / 8), wav->numChannels, wav->bitsPerSample, spectrum.data() + i);
	}
}

void Wave::PrintInfo()
{
	printf("Path: %s\n", path.c_str());
	printf("Name: %s\n", name.c_str());
	printf("Channels: %u\n", numChannels);
	printf("SampleRate: %u\n", sampleRate);
	printf("SampleSize: %u\n", sampleSize);
	printf("BitsPerSample: %u\n", bitsPerSample);
	printf("ByteSize: %u\n", byteSize);
	printf("Duration: %dm%ds\n", (uint32_t)duration / 60, (uint32_t)duration % 60);
	//printf("SampleBlockSize: %d\n", blockSize);
	//printf("SampleBlockNum: %d (last block: %d - %d)\n", blocks.size(), blocks.back().byteSize, blocks.back().sampleSize);
}

void Wave::Load(const char* path)
{
	std::ifstream fs(path, std::ios::in | std::ios::binary);
	if (!fs)
	{
		std::cerr << "ERROR: can't open file (" << path << ")\n";
		abort();
	}
	this->path = std::string(path);
	pathU8 = GbkToUtf8(this->path);
	name = PathToName(path);
	nameU8 = GbkToUtf8(name);
	int idx = 0;

	header = new Header;

	header->riff = new ChunkRIFF;
	fs.read((char*)header->riff, sizeof(ChunkRIFF));
	if (header->riff->chunkID != MakeWaveID<'R', 'I', 'F', 'F'>::value)
	{
		std::cerr << "ERROR: riff id wrong (" << path << ")\n";
	}
	idx += 12;
	fs.seekg(idx, std::ios::beg);

	header->format = new ChunkFormat;
	fs.read((char*)header->format, sizeof(ChunkFormat));
	if (header->format->chunkID != MakeWaveID<'f', 'm', 't', ' '>::value)
	{
		std::cerr << "ERROR: format id wrong (" << path << ")\n";
	}
	idx += header->format->chunkSize + 8;
	fs.seekg(idx, std::ios::beg);

	while (1)
	{
		Chunk* chunk = new Chunk;
		fs.read((char*)chunk, sizeof(Chunk));
		header->chunks.push_back(chunk);
		if (chunk->chunkID == MakeWaveID<'d', 'a', 't', 'a'>::value)
		{
			data = new int8_t[chunk->chunkSize];
			byteSize = chunk->chunkSize;			
			fs.read((char*)data, byteSize);
			break;
		}		
		idx += chunk->chunkSize + 8;
		fs.seekg(idx, std::ios::beg);
	}
	numChannels = header->format->numChannels;
	sampleRate = header->format->sampleRate;
	sampleSize = byteSize / header->format->numChannels / (header->format->bitsPerSample / 8);
	bitsPerSample = header->format->bitsPerSample;
	duration = (float)sampleSize / header->format->sampleRate;
	byteRate = sampleRate * numChannels * (bitsPerSample / 8);
	blockSize = 102400;
	GenerateBlocks();
	//printf("\nPath: %s\n", this->path);
	PrintInfo();

	fs.close();

	//PrintInfo();
}

void Wave::GenerateBlocks()
{
	int8_t* dataStart = data;
	int8_t* blockCursor = data;
	int8_t* dataEnd = data + byteSize;
	while (1)
	{
		if (dataEnd - blockCursor >= blockSize)
		{
			blocks.push_back({ this, (int)(blockCursor - dataStart), blockSize, blockSize / numChannels / (bitsPerSample / 8), blockCursor });
			blockCursor += blockSize;
		}
		else
		{
			int size = (int)(dataEnd - blockCursor);
			blocks.push_back({ this, (int)(blockCursor - dataStart), size, size / numChannels / (bitsPerSample / 8), blockCursor });
			break;
		}
	}
}

std::string WinUtf8ToGbk(std::string utf8)
{
	// ½«UTF-8×ª»»Îª¿í×Ö·û£¨UTF-16£©
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	if (wideLen == 0) 
	{
		throw std::runtime_error("UTF-8 ×ª¿í×Ö·ûÊ§°Ü¡£");
	}

	std::wstring wideStr(wideLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wideStr[0], wideLen);

	// ½«¿í×Ö·û£¨UTF-16£©×ª»»ÎªGBK
	int gbkLen = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (gbkLen == 0) 
	{
		throw std::runtime_error("¿í×Ö·û×ªGBKÊ§°Ü¡£");
	}

	std::string gbkStr(gbkLen, 0);
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &gbkStr[0], gbkLen, nullptr, nullptr);

	// È¥³ýÄ©Î²µÄ¿Õ×Ö·û
	gbkStr.pop_back();
	return gbkStr;
}

// GBK ×ª UTF-8
std::string WinGbkToUtf8(std::string gbk)
{
	// ½«GBK×ª»»Îª¿í×Ö·û£¨UTF-16£©
	int wideLen = MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, nullptr, 0);
	if (wideLen == 0) 
	{
		throw std::runtime_error("GBK ×ª¿í×Ö·ûÊ§°Ü¡£");
	}

	std::wstring wideStr(wideLen, 0);
	MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, &wideStr[0], wideLen);

	// ½«¿í×Ö·û£¨UTF-16£©×ª»»ÎªUTF-8
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (utf8Len == 0) 
	{
		throw std::runtime_error("¿í×Ö·û×ªUTF-8Ê§°Ü¡£");
	}

	std::string utf8Str(utf8Len, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Len, nullptr, nullptr);

	// È¥³ýÄ©Î²µÄ¿Õ×Ö·û
	utf8Str.pop_back();
	return utf8Str;
}

std::string GbkToUtf8(std::string& str)
{
	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t>F;
	static std::wstring_convert<F>strC(new F("Chinese"));
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> strCnv;
	return strCnv.to_bytes(strC.from_bytes(str));
}

std::string PathToName(std::string path)
{
	const char* p = path.c_str();
	int size = strlen(p);
	int dot = size -1;
	for (int i = size - 1; i >= 0; i--)
	{
		if (path[i] == '.')
		{
			dot = i;
		}

		if (path[i] == '\\')
		{
			/*
			name = new char[size - i + 1];
			memcpy(name, path + i + 1, size - i);
			name[size - i] = '\0';
			printf("%s\n", name);
			*/
			std::string name(p + i + 1, (size_t)(dot - i - 1));
			return name;
		}
	}
}

void StereoToMono(Wave& stereo, Wave& mono)
{
	//mono.header = new Header(*stereo.header);
	//mono.header->format->numChannels = 1;
	//mono.header->format->byteRate = mono.header->format->sampleRate * mono.header->format->numChannels * mono.header->format->bitsPerSample / 8;
	mono.path = stereo.path;
	mono.pathU8 = stereo.pathU8;
	mono.name = stereo.name;
	mono.nameU8 = stereo.nameU8;
	mono.numChannels = 1;
	mono.sampleRate = stereo.sampleRate;
	mono.byteRate = stereo.byteRate / 2;
	mono.sampleSize = stereo.sampleSize;
	mono.bitsPerSample = stereo.bitsPerSample;
	mono.byteSize = stereo.byteSize / 2;
	mono.duration = stereo.duration;	

	mono.data = new int8_t[mono.byteSize];
	for (uint32_t i = 0; i < mono.byteSize; i += 2)
	{		
		int16_t leftSample = *(int16_t*)(stereo.data + i * 2);
		int16_t rightSample = *(int16_t*)(stereo.data + (i + 1) * 2);
		*(int16_t*)(mono.data + i) = (leftSample + rightSample) / 2;
	}
	mono.blockSize = 102400;
	mono.GenerateBlocks();
}

void Filter(Wave& track)
{
	uint16_t* tdata = (uint16_t*)track.data;
	uint16_t* fdata = (uint16_t*)malloc(track.byteSize);
	memset(fdata, 0, track.byteSize);
	float factor = 3.14 * 2 * 10000;
	for (int i = 0; i < track.sampleSize; i++)
	{
		float t = (float)i / track.sampleRate;
		float f = 1 + abs(sin(factor * t));
		tdata[i] = tdata[i] * f;
	}
}