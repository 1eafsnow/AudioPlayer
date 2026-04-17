#include "Spectrum.h"
#include "Wave.h"

SpectrumExtractor* Extractor = new SpectrumExtractor(44100);
SpectrumExtractor* GetSpectrumExtractor()
{
	return Extractor;
}

double Spectrum::GetDB(double frequency, int sampleRate)
{
	int idx = frequency / (sampleRate / 2) * (FT_WINDOW_SIZE / 2);
	//printf("Frequency: %.2f Hz, Index: %d\n", frequency, idx);
	//double magnitude = magnitudes[idx];
	double magnitude = 0.0;
	for (int i = idx - 10; i <= idx + 10; i++)
	{
		magnitude += magnitudes[i] / 20;
	}
	return 20 * log10(magnitude / (32677 * 32677));
	//return magnitude / (sampleRate * 2);
}

double Spectrogram::GetMagnitude(int samleOffset, double frequency)
{
	int spectrumIdx = samleOffset / (FT_WINDOW_SIZE / 2);
	int frequencyIdx = frequency / (sampleRate / 2) * (FT_WINDOW_SIZE / 2);
	//printf("spectrumIdx: %d, frequencyIdx: %d\n", spectrumIdx, frequencyIdx);
	double magnitude1 = (float)spectrumFrames[spectrumIdx].spectrum.magnitudes[frequencyIdx];
	if (spectrumIdx == spectrumFrames.size() - 1)
	{
		return 20 * log10(magnitude1);
	}
	double magnitude2 = (float)spectrumFrames[spectrumIdx + 1].spectrum.magnitudes[frequencyIdx];
	double offset = (samleOffset % (FT_WINDOW_SIZE / 2)) / (double)(FT_WINDOW_SIZE / 2);
	return 20 * log10(magnitude1 * (1 - offset) + magnitude2 * offset);
}

SpectrumExtractor::SpectrumExtractor(int sampleRate) : sampleRate(sampleRate)
{
	fft_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FT_WINDOW_SIZE);
	fft_output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FT_WINDOW_SIZE);

	plan = fftw_plan_dft_1d(FT_WINDOW_SIZE, fft_input, fft_output, FFTW_FORWARD, FFTW_ESTIMATE);
	
}

void SpectrumExtractor::Execute()
{
	fftw_execute(plan);
	/*
	for (int i = 0; i < fftSize; i++)
	{
		double magnitude = sqrt(fft_output[i][0] * fft_output[i][0] + fft_output[i][1] * fft_output[i][1]);
		double frequency = ((double)i / fftSize) * sampleRate;
		
		printf("Frequency: %.2f Hz, Magnitude: %.4f\n", frequency, magnitude);
	}	
	*/
}

void SpectrumExtractor::Extract()
{
	for (int i = 0; i < FT_WINDOW_SIZE; i++)
	{
		fft_input[i][0] = sin(i * (2 * 3.1415926535 / 256) * 50) + sin(i * (2 * 3.1415926535 / 256) * 100);
		fft_input[i][1] = 0.0;
	}
	Execute();
	double out[FT_WINDOW_SIZE / 2];
	for (int i = 0; i < FT_WINDOW_SIZE / 2; i++)
	{
		double magnitude = sqrt(fft_output[i][0] * fft_output[i][0] + fft_output[i][1] * fft_output[i][1]);
		double frequency = ((double)i / (FT_WINDOW_SIZE / 2)) * 128;
		printf("Frequency: %.2f Hz, Magnitude: %.4f\n", frequency, magnitude);
	}
}

void SpectrumExtractor::Extract(Wave* wav, int sampleOffet, Spectrum* spectrum)
{		
	if (sampleOffet + FT_WINDOW_SIZE >= wav->sampleSize)
	{
		return;
	}
	int16_t* wavData = (int16_t*)wav->data;
	for (int i = 0; i < FT_WINDOW_SIZE; i++)
	{		
		fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * wavData[sampleOffet + i];
		fft_input[i][1] = 0.0;
	}

	Execute();
		
	for (int i = 0; i < FT_WINDOW_SIZE / 2; i++)
	{		
		spectrum->magnitudes[i] = sqrt(fft_output[i][0] * fft_output[i][0] + fft_output[i][1] * fft_output[i][1]);
	}
}

void SpectrumExtractor::Extract(int16_t* sampleData, Spectrum* spectrum)
{	
	for (int i = 0; i < FT_WINDOW_SIZE; i++)
	{
		fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * sampleData[i];
		fft_input[i][1] = 0.0;
	}

	Execute();

	for (int i = 0; i < FT_WINDOW_SIZE / 2; i++)
	{
		spectrum->magnitudes[i] = sqrt(fft_output[i][0] * fft_output[i][0] + fft_output[i][1] * fft_output[i][1]);
	}
}

void SpectrumExtractor::Extract(int8_t* data, int numChannels, int bitsPerSample, Spectrum* spectrum)
{
	switch (numChannels)
	{
	case 1:
		switch (bitsPerSample)
		{
		case 8:
			for (int i = 0; i < FT_WINDOW_SIZE; i++)
			{
				fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * data[i];
				fft_input[i][1] = 0.0;
			}
			break;

		case 16:
			for (int i = 0; i < FT_WINDOW_SIZE; i++)
			{
				int16_t* v = ((int16_t*)data) + i;
				fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * *v;
				fft_input[i][1] = 0.0;
			}
			break;
		}

		break;

	case 2:
		switch (bitsPerSample)
		{
		case 8:
			for (int i = 0; i < FT_WINDOW_SIZE; i++)
			{
				int8_t v = (data[i * 2] + data[i * 2 + 1]) / 2;
				fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * v;
				fft_input[i][1] = 0.0;
			}
			break;

		case 16:
			for (int i = 0; i < FT_WINDOW_SIZE; i++)
			{
				int16_t* l = ((int16_t*)data) + i;
				fft_input[i][0] = HammingWindow(i, FT_WINDOW_SIZE) * ((*l + *(l + 1)) / 2);
				fft_input[i][1] = 0.0;
			}
			break;
		}
		break;
	}

	Execute();

	for (int i = 0; i < FT_WINDOW_SIZE / 2; i++)
	{
		spectrum->magnitudes[i] = sqrt(fft_output[i][0] * fft_output[i][0] + fft_output[i][1] * fft_output[i][1]);
	}
}

void SpectrumExtractor::Extract(Wave* wav, Spectrogram* spectrogram)
{
	int sampleCount = 0;	
	while (sampleCount + FT_WINDOW_SIZE <= wav->sampleSize)
	{
		spectrogram->spectrumFrames.push_back({ sampleCount, Spectrum() });
		Spectrum* spectrum = &spectrogram->spectrumFrames.back().spectrum;
		Extract(wav, sampleCount, spectrum);
		sampleCount += (FT_WINDOW_SIZE / 2);
	}
}

void SpectrumExtractor::Inject(Spectrum* spectrum, int16_t* sampleData, int sampleRate)
{
	for (int i = 0; i < SPECTRUM_SIZE; i++)
	{
		double magnitude = spectrum->magnitudes[i] / (sampleRate * 2);
		double frequency = (i + 1.0) / (SPECTRUM_SIZE) * (sampleRate / 2);
		for (int j = 0; j < FT_WINDOW_SIZE; j++)
		{
			sampleData[j] = sin(j * (2 * 3.1415926535 / sampleRate) * frequency) * magnitude;
		}
	}
}

double HammingWindow(int n, int N)
{
	return 0.54 - 0.46 * cos(2 * PI * n / (N - 1));
}