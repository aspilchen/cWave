/** @file wavedata.h
 *  @brief Load and store wave data in memory.
 *  @version 1.0
 *  @date 2022/04/15
 *  @author Adam Spilchen
 * 
 *  @todo Create tests.
 */

#ifndef WAVEDATA_H
#define WAVEDATA_H

#define SAMPLE_SIZE   (sizeof(short)) 	// bytes per sample

#include <stddef.h>


/// @todo fill in header data.
typedef struct {
	size_t numSamples;
	size_t maxSize;
	short *pointData;
} wavedata_t;


enum WaveErr {
	WAVE_FILE_ERR = -3
	,WAVE_MEMORY_ERR = -2
	,WAVE_READ_ERR = -1
};


/// @brief Initialize wave data to hold size number of samples.
/// @return Return true upon success. False likely indicates a memory issue.
/// @param wave Pointer to wavedata being initialized.
/// @param size Number of samples to initialize in memory.
int wavedata_init(wavedata_t *wave, size_t size);


/// @brief Destroy wavedata.
/// @param wave Pointer to wavedata being destroyed.
void wavedata_destroy(wavedata_t *wave);


/// @brief Dynamically allocate memory and read wave data from a file.
/// @param wave Pointer to wavedata to be written to.
/// @param fileName Path to file containing wave data.
/// @return Number of samples saved upon success. WaveErr on failure.
/// @warning It is the users responsibility to destroy the wavedata after use.
int wavedata_readWaveFile(wavedata_t *wave, char *fileName);

#endif