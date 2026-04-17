#pragma once

#include <fftw3.h>
#include <string.h>
#include <math.h>
#include <vector>

#define PI 3.14159265358979323846
#define FT_WINDOW_SIZE 1024
#define SPECTRUM_SIZE (FT_WINDOW_SIZE / 2)
const double FREQUENCY[20] {50.0, 69.0, 94.0, 129.0, 176.0, 241.0, 331.0, 453.0, 620.0, 850.0, 1200.0, 1600.0, 2200.0, 3000.0, 4100.0, 5600.0, 7700.0, 11000.0, 14000.0, 20000.0};

class Wave;
class Spectrum
{
public:
	double magnitudes[FT_WINDOW_SIZE / 2];

	double GetDB(double frequency, int sampleRate);
};

struct SpectrumFrame
{
	int sampleOffset;
	Spectrum spectrum;
};

class Spectrogram
{
public:
	int sampleRate;
	std::vector<SpectrumFrame> spectrumFrames;
	double GetMagnitude(int samleOffset, double frequency);
};

class SpectrumExtractor
{
public:
	int sampleRate;

	fftw_complex* fft_input;
	fftw_complex* fft_output;

	fftw_plan plan;	

	SpectrumExtractor(int sampleRate);
	void Execute();
	void Extract();
	void Extract(Wave* wav, int sampleOffet, Spectrum* spectrum);
	void Extract(int16_t* sampleData, Spectrum* spectrum);
	void Extract(int8_t* data, int numChannels, int bitsPerSample, Spectrum* spectrum);
	void Extract(Wave* wav, Spectrogram* spectrogram);

	void Inject(Spectrum* spectrum, int16_t* sampleData, int sampleRate);
};

double HammingWindow(int n, int N);

SpectrumExtractor* GetSpectrumExtractor();