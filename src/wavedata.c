#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../headers/wavedata.h"

/******************************************************************************
 **              INTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/
static int   wavedata_initImpl         (wavedata_t *wave, size_t size);
static void   wavedata_destroyImpl      (wavedata_t *wave);
static int    wavedata_readWaveFileImpl (wavedata_t *wave, char *fileName);
static size_t wave_calculateNumSamples  (FILE       *file, size_t headerSize);
static void   wave_skipWaveHeader       (FILE       *file, size_t headerSize);

/******************************************************************************
 **              FUNCTION DEFINITIONS
 ******************************************************************************/
int wavedata_init(wavedata_t *wave, size_t size)
{
	assert(wave);
	assert(size > 0);
	return wavedata_initImpl(wave, size);
}

void wavedata_destroy(wavedata_t *wave)
{
	assert(wave);
	assert(wave->pointData);
	wavedata_destroyImpl(wave);
}

int wavedata_readWaveFile(wavedata_t *wave, char *fileName)
{
	assert(wave);
	assert(fileName);
	return wavedata_readWaveFileImpl(wave, fileName);
}

void wave_destroy(wavedata_t *wave)
{
	assert(wave);
	assert(wave->pointData);
	wavedata_destroyImpl(wave);
}

/******************************************************************************
 **              INTERNAL FUNCTIONS
 ******************************************************************************/
static int wavedata_initImpl(wavedata_t *wave, size_t size)
{
	short *pointData = malloc(SAMPLE_SIZE * size);

	if(!pointData) {
		return -1;
	}

	wave->pointData = pointData;
	wave->numSamples = 0;
	wave->maxSize = size;
	return 0;
}

static void wavedata_destroyImpl(wavedata_t *wave)
{
	free(wave->pointData);
	wave->pointData = NULL;
	wave->numSamples = 0;
	wave->maxSize = 0;
}

static int wavedata_readWaveFileImpl(wavedata_t *wave, char *fileName)
{

	// Wave file has 44 bytes of header data. This code assumes file
	// is correct format.
	const int WAVE_HEADER_SIZE = 44;

	// Open file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fclose(file);
		return WAVE_FILE_ERR;
	}

	// Allocate Space
	size_t numSamples = wave_calculateNumSamples(file, WAVE_HEADER_SIZE);
	short *pointData = malloc(numSamples * SAMPLE_SIZE);
	if (pointData == NULL) {
		fclose(file);
		return WAVE_MEMORY_ERR;
	}

	// Read data
	wave_skipWaveHeader(file, WAVE_HEADER_SIZE);
	int samplesRead = fread(pointData, SAMPLE_SIZE, numSamples, file);
	if (samplesRead != numSamples) {
		fclose(file);
		return WAVE_READ_ERR;
	}

	// Cleanup and move data to wave
	fclose(file);
	wave->pointData = pointData;
	wave->numSamples = numSamples;
	wave->maxSize = numSamples;
	return samplesRead;
}

static size_t wave_calculateNumSamples(FILE *file, size_t headerSize)
{
	fseek(file, 0, SEEK_END);
	size_t sizeInBytes = ftell(file);
	size_t nSamples = (sizeInBytes - headerSize) / SAMPLE_SIZE;
	return nSamples;
}

static void wave_skipWaveHeader(FILE *file, size_t headerSize)
{
	fseek(file, headerSize, SEEK_SET);
}