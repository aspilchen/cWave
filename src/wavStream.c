#include <stdlib.h>

#include "../headers/wavStream.h"

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS                                     
******************************************************************************/

/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES                                   
******************************************************************************/

static struct WavStream* wavstream_openImpl(struct MixerChannel *channel, size_t channelId);
static void wavstream_closeImpl(struct WavStream *stream);
static bool wavstream_isEmptyImpl(struct WavStream *stream);
static short wavstream_readNextImpl(struct WavStream *stream);

/******************************************************************************
**              INTERNAL VARIABLES                                             
******************************************************************************/

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
struct WavStream* wavstream_open(struct MixerChannel *channel, size_t channelId)
{
	return wavstream_openImpl(channel, channelId);
}

void wavstream_close(struct WavStream *stream)
{
	wavstream_closeImpl(stream);
}

bool wavstream_isEmpty(struct WavStream *stream)
{
	wavstream_isEmptyImpl(stream);
}

short wavstream_readNext(struct WavStream *stream)
{
	return wavstream_readNextImpl(stream);
}

/******************************************************************************
**              INTERNAL FUNCTION DEFINITIONS                                  
******************************************************************************/
static struct WavStream* wavstream_openImpl(struct MixerChannel *channel, size_t channelId)
{
	struct WavStream *strm = malloc(sizeof(*strm));
	if(!strm) {
		return 0;
	}

	strm->front = channel->start;
	strm->end = channel->end;
	strm->remaining = strm->end - strm->front;
	strm->channelId = channelId;
	return strm;
}

static void wavstream_closeImpl(struct WavStream *stream)
{
	free(stream);
}

static bool wavstream_isEmptyImpl(struct WavStream *stream)
{
	return stream->front >= stream->end;
}

static short wavstream_readNextImpl(struct WavStream *stream)
{
	if(wavstream_isEmptyImpl(stream)) {
		return 0;
	}

	short val = *stream->front;
	stream->front++;
	return val;
}