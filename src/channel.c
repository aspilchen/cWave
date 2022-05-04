#include <string.h>
#include <assert.h>
#include <limits.h>

#include "../headers/channel.h"


/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES                                   
******************************************************************************/
static void channel_loadImpl(struct MixerChannel *channel, wavedata_t *wav);
static void channel_clearImpl(struct MixerChannel *channel);
static int channel_setStartImpl(struct MixerChannel *channel, size_t start);
static int channel_setEndImpl(struct MixerChannel *channel, size_t end);

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
void channel_load(struct MixerChannel *channel, wavedata_t *wav)
{
	assert(wav->pointData);
	assert(wav->numSamples > 0);
	if(!wav->pointData || wav->numSamples == 0) {
		return;
	}

	channel_loadImpl(channel, wav);
}

void channel_clear(struct MixerChannel *channel)
{
	channel_clearImpl(channel);
}

int channel_setStart(struct MixerChannel *channel, size_t start)
{
	return channel_setStartImpl(channel, start);
}

int channel_setEnd(struct MixerChannel *channel, size_t end)
{
	return channel_setEndImpl(channel, end);
}

/******************************************************************************
**              INTERNAL FUNCTION DEFINITIONS                                  
******************************************************************************/
static void channel_loadImpl(struct MixerChannel *channel, wavedata_t *wav)
{
	channel->pData = wav->pointData;
	channel->start = wav->pointData;
	channel->end = wav->pointData + wav->numSamples;
	channel->nPoints = wav->numSamples;
}

static void channel_clearImpl(struct MixerChannel *channel)
{
	channel->pData = 0;
	channel->start = 0;
	channel->end = 0;
	channel->nPoints = 0;
}


static int channel_setStartImpl(struct MixerChannel *channel, size_t start)
{
	short *strt = channel->pData + start;
	if(strt > channel->end) {
		return 0;
	}

	if(start > channel->nPoints) {
		return 0;
	}

	channel->start = strt;
	return 1;
}

static int channel_setEndImpl(struct MixerChannel *channel, size_t end)
{
	short *endPtr = channel->pData + end;
	if(endPtr < channel->start) {
		return 0;
	}

	if(end > channel->nPoints) {
		return 0;
	}

	channel->end = endPtr;
	return 1;
}