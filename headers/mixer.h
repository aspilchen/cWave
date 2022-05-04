#ifndef MIXER_H
#define MIXER_H

#include "../headers/wavedata.h"

#include <stdbool.h>
#include <stddef.h>

void mixer_init(size_t nChannels);
void mixer_cleanup(void);
void mixer_loadWaveIntoChannel(size_t channel, wavedata_t *wav);
void mixer_turnChannelOn(size_t channel);
void mixer_turnChannelOff(size_t channel);
void mixer_setChannelStart(size_t channel, size_t start);
void mixer_setChannelEnd(size_t channel, size_t end);

void mixer_applyChannelFilter(size_t channel, short (*filter)(short));
void mixer_speedUp(void);
void mixer_speedDown(void);

void mixer_queueChannel(size_t channel);
size_t mixer_mix(short *output, size_t buffsize);
bool mixer_channelHasWave(size_t channel);
bool mixer_isChannelPlaying(size_t channel);

#endif
