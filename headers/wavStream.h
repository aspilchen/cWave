#ifndef WAVSTREAM_H
#define WAVSTREAM_H

#include <stddef.h>
#include <stdbool.h>

#include "channel.h"

struct WavStream {
	short *front;
	short *end;
	size_t channelId;
	size_t remaining;
};


struct WavStream* wavstream_open(struct MixerChannel *channel, size_t channelId);
void wavstream_close(struct WavStream *stream);
bool wavstream_isEmpty(struct WavStream *strm);
short wavstream_readNext(struct WavStream *stream);


#endif