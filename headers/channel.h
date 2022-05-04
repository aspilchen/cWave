#ifndef CHANNEL_H
#define CHANNEL_H

#include <stddef.h>
#include <stdbool.h>

#include "wavStream.h"
#include "wavedata.h"

struct MixerChannel {
	short *pData;
	short *start;
	short *end;
	size_t nPoints;
};


void channel_load(struct MixerChannel *channel, wavedata_t *wav)
	__attribute__((nonnull));


void channel_clear(struct MixerChannel *channel)
	__attribute__((nonnull));


int channel_setStart(struct MixerChannel *channel, size_t start)
	__attribute__((nonnull));


int channel_setEnd(struct MixerChannel *channel, size_t end)
	__attribute__((nonnull));


#endif
