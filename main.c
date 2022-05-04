#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "wavedata.h"
#include "channel.h"
#include "mixer.h"
#include "player.h"

#define BSIZE 100
short buffer[BSIZE];

pthread_mutex_t mainLock = PTHREAD_MUTEX_INITIALIZER;

void main_exit(void) {
	pthread_mutex_unlock(&mainLock);
}

int main() {
	player_init();
	
	pthread_mutex_lock(&mainLock);
	pthread_mutex_lock(&mainLock);

	// struct PlayerChannel ch;
	// mixer_init(4);
	// mixer_setOutputBuffer(buffer, BSIZE);
// 
	// wavedata_t wav;
	// wavedata_readWaveFile(&wav, "100060__menegass__gui-drum-splash-hard.wav");
	// mixer_loadWave(0, &wav);
// 
	// channel_loadWave(&ch, &wav);
	// while(!channel_atEnd(&ch)) {
		// channel_getWaveChunk(&ch, buffer, BSIZE);
	// }


	player_cleanup();
	exit(EXIT_SUCCESS);
}