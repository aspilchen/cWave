#include "../headers/wavStream.h"
#include "../headers/channel.h"
#include "../headers/mixer.h"
#include "../headers/dynarrExperiment.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

/******************************************************************************
**              INTERNAL TYPES
******************************************************************************/
typedef struct MixerChannel Channel;
typedef short (*ChannelFilter) (short);

#ifndef STREAM__ARR__
#define STREAM__ARR__
typedef struct WavStream *WavStream;
dynarr_declare(WavStream);
#endif

/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES                                   
******************************************************************************/
static void mixer_initImpl(size_t nChannels);
static void mixer_cleanupImpl(void);
static void mixer_loadWaveIntoChannelImpl(size_t channel, wavedata_t *wav);
static void mixer_turnChannelOnImpl(size_t channel);
static void mixer_turnChannelOffImpl(size_t channel);
static void mixer_setChannelStartImpl(size_t channel, size_t start);
static void mixer_setChannelEndImpl(size_t channel, size_t end);
static void mixer_queueChannelImpl(size_t channel);
static size_t mixer_mixImpl(short *output, size_t buffsize);
static bool mixer_channelHasWaveImpl(size_t channel);
static bool mixer_isChannelPlayingImpl(size_t channel);

static void mixer_mixStream(WavStream strm, short *outBuff, size_t buffSize);

static void mixer_FFT();

/******************************************************************************
**              INTERNAL VARIABLES                                             
******************************************************************************/
static size_t numChannels = 0;
static Channel *channels = 0;
static bool *channelIsOn = 0;
static bool *channelIsPlaying = 0;
static Dynarr(WavStream) streams;
ChannelFilter *filters;

// static const size_t outSize    = 1024 * 2;
// static const size_t windowSize = 32;
// static const size_t hop        = 16;

// static short outputBuffer[outSize];
// static short *readPtr = outputBuffer;
// static short *writePtr = outputBuffer;

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
void mixer_init(size_t nChannels)
{
	mixer_initImpl(nChannels);
}

void mixer_cleanup(void)
{
	mixer_cleanupImpl();
}

void mixer_loadWaveIntoChannel(size_t channel, wavedata_t *wav)
{
	mixer_loadWaveIntoChannelImpl(channel, wav);
}

void mixer_turnChannelOn(size_t channel)
{
	mixer_turnChannelOnImpl(channel);
}

void mixer_turnChannelOff(size_t channel)
{
	mixer_turnChannelOffImpl(channel);
}

void mixer_setChannelStart(size_t channel, size_t start)
{
	mixer_setChannelStartImpl(channel, start);
}

void mixer_setChannelEnd(size_t channel, size_t end)
{
	mixer_setChannelEndImpl(channel, end);
}

void mixer_applyChannelFilter(size_t channel, short (*filter)(short))
{
	filters[channel] = filter;
}

void mixer_queueChannel(size_t channel)
{
	mixer_queueChannelImpl(channel);
}

size_t mixer_mix(short *output, size_t buffsize)
{
	return mixer_mixImpl(output, buffsize);
}

bool mixer_channelHasWave(size_t channel)
{
	mixer_channelHasWaveImpl(channel);
}

bool mixer_isChannelPlaying(size_t channel)
{
	mixer_isChannelPlayingImpl(channel);
}

/******************************************************************************
**              INTERNAL FUNCTION DEFINITIONS                                           
******************************************************************************/
static void mixer_initImpl(size_t nChannels)
{
	numChannels = nChannels;
	channels = malloc(nChannels * sizeof(*channels));
	channelIsOn = malloc(nChannels * sizeof(*channelIsOn));
	channelIsPlaying = malloc(nChannels * sizeof(*channelIsPlaying));
	filters = calloc(numChannels, sizeof(*filters));
	dynarr_init(&streams);
	for(int i = 0; i < arrsize(&streams); i++) {
		dynarr_put(&streams, i, 0);
	}
}

static void mixer_cleanupImpl(void)
{
	free(channels);
	free(channelIsOn);
	free(channelIsPlaying);
	free(filters);
	dynarr_destroy(&streams);
}

static void mixer_loadWaveIntoChannelImpl(size_t channel, wavedata_t *wav)
{
	channel_load(channels + channel, wav);
}

static void mixer_turnChannelOnImpl(size_t channel)
{
	channelIsOn[channel] = true;
}

static void mixer_turnChannelOffImpl(size_t channel)
{
	channelIsOn[channel] = false;
}

static void mixer_setChannelStartImpl(size_t channel, size_t start)
{
	channel_setStart(channels + channel, start);
}

static void mixer_setChannelEndImpl(size_t channel, size_t end)
{
	channel_setEnd(channels + channel, end);
}

static void mixer_queueChannelImpl(size_t channel)
{
	if(!channelIsOn[channel]) {
		return;
	}
	WavStream strm = wavstream_open(channels + channel, channel);
	WavStream tmp = 0;
	for(int i = 0; i < arrsize(&streams); i++) {
		tmp = dynarr_get(&streams, i);
		if(!tmp) {
			dynarr_put(&streams, i, strm);
			return;
		}
	}
	dynarr_pushBack(&streams, strm);
}

static size_t mixer_mixImpl(short *output, size_t buffsize)
{
	memset(output, 0, SAMPLE_SIZE * buffsize);
	WavStream strm;
	short point;
	long pointSum;
	int nStreams = 0;
	for(int i = 0; i < arrsize(&streams); i++) {
		strm = dynarr_get(&streams, i);

		if(!strm) {
			continue;
		}

		if(!channelIsOn[strm->channelId]) {
			continue;
		}

		mixer_mixStream(strm, output, buffsize);
		nStreams++;

		if(wavstream_isEmpty(strm)) {
			wavstream_close(strm);
			dynarr_put(&streams, i, 0);
		}
	}
	return nStreams;
}

static bool mixer_channelHasWaveImpl(size_t channel)
{
	return false;
}

static bool mixer_isChannelPlayingImpl(size_t channel)
{
	return false;
}


static void mixer_mixStream(WavStream strm, short *outBuff, size_t buffSize)
{
	int i = 1;
	short point;
	long pointSum = 0;
	static int a = 0;
	static int b = 0;
	while(i < buffSize && !wavstream_isEmpty(strm)) {
		point = wavstream_readNext(strm);
		pointSum = outBuff[i] + point;
		if(pointSum > SHRT_MAX) {
			pointSum = SHRT_MAX;
		} else if (pointSum < SHRT_MIN) {
			pointSum = SHRT_MIN;
		}
		outBuff[i] = (short)pointSum;
		i += 2;
	}
}