#include "../headers/player.h"
#include "../headers/wavedata.h"
#include "../headers/mixer.h"

#include <stdio.h>

#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for volume

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS                                     
******************************************************************************/
#define DEFAULT_VOLUME 80
#define MAX_VOLUME 100
#define SAMPLE_RATE 44100
#define OUTPUT_CHANNELS 1
#define MIXER_CHANNELS 3

/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES                                   
******************************************************************************/
static void player_initImpl(void);
static void player_cleanupImpl(void);
static int player_getVolumeImpl(void);
static void player_setVolumeImpl(int newVolume);
static void* player_outputThreadFunc(void *arg);

/******************************************************************************
**              INTERNAL VARIABLES                                             
******************************************************************************/
wavedata_t wavedata[2];
static size_t mbuffsize = 50;
static short mixerBuff[50];
static size_t outBuffSize = 0;
static short *outBuffA;
static short *outBuffB;
static snd_pcm_t *handle;
static pthread_t outThreadId;

static int volume = 0;
static bool isRunning = false;

short vol = 1;
short ffunc(short val) {
	int tmp = val * vol;
	if(tmp > SHRT_MAX) {
		tmp = SHRT_MAX;
	} else if(tmp < SHRT_MIN) {
		tmp = SHRT_MIN;
	}
	return (short)tmp;
}

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
void player_init(void)
{
	player_initImpl();
}

void player_cleanup(void)
{
	player_cleanupImpl();
}

int player_getVolume(void)
{
	return player_getVolumeImpl();
}

void player_setVolume(int newVolume)
{
	player_setVolumeImpl(newVolume);
}

/******************************************************************************
**              INTERNAL FUNCTION DEFINITIONS                                  
******************************************************************************/
static void player_initImpl(void)
{
	// player_setVolumeImpl(DEFAULT_VOLUME);
    snd_pcm_hw_params_t *hwparams;

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	// int exact_rate = SAMPLE_RATE;
	// int periods = 2;
	// snd_pcm_uframes_t periodsize = 1024;
	// int latency = periodsize * periods / (SAMPLE_RATE * SAMPLE_SIZE);
// 
    // snd_pcm_hw_params_alloca(&hwparams);
	// snd_pcm_hw_params_any(handle, hwparams);
	// snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	// snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S16_LE);
	// snd_pcm_hw_params_set_rate_near(handle, hwparams, &exact_rate, 0);
	// snd_pcm_hw_params_set_channels(handle, hwparams, OUTPUT_CHANNELS);
	// snd_pcm_hw_params_set_periods(handle, hwparams, 2, 0);
	// snd_pcm_hw_params_set_buffer_size(handle, hwparams, (periodsize * periods)/SAMPLE_SIZE);
	// err =snd_pcm_hw_params(handle, hwparams);
		// Configure parameters of PCM output
		err = snd_pcm_set_params(handle,
								 SND_PCM_FORMAT_S16_LE,
								 SND_PCM_ACCESS_RW_INTERLEAVED,
								 OUTPUT_CHANNELS,
								 SAMPLE_RATE,
								 1,		 // Allow software resampling
								 50000); // 0.05 seconds per buffer

	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Get info on the buffers:
 	unsigned long buffer_size = 0;
	snd_pcm_get_params(handle, &buffer_size, &outBuffSize);
	printf("%ld %ld\n", buffer_size, outBuffSize);
	//  outBuffSize = buffer_size;

	// Allocate playback buffer:
	outBuffA = malloc(outBuffSize * sizeof(*outBuffA));
	outBuffB = malloc(outBuffSize * sizeof(*outBuffB));

	wavedata_readWaveFile(wavedata, "around_the_world-atc.wav");
	// wavedata_readWaveFile(wavedata+1, "beatbox-wav-files/100060__menegass__gui-drum-splash-hard.wav");
	mixer_init(MIXER_CHANNELS);
	mixer_loadWaveIntoChannel(0,wavedata);
	// mixer_loadWaveIntoChannel(1, wavedata+1);
	// Launch playback thread:
	isRunning = true;
	pthread_create(&outThreadId, NULL, player_outputThreadFunc, NULL);
}


static void player_cleanupImpl(void)
{
	isRunning = false;
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	fflush(stdout);
}


static int player_getVolumeImpl()
{
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
static void player_setVolumeImpl(int newVolume)
{
	if (newVolume < 0 || newVolume > MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}


static void* player_outputThreadFunc(void *arg)
{
	mixer_turnChannelOn(0);
	mixer_queueChannel(0);
	// mixer_turnChannelOff(1);
	// mixer_applyChannelFilter(0, ffunc);
	const size_t overlap = 0;
	short *a = outBuffA;
	short *b = outBuffA;
	while (true) {
		mixer_mix(outBuffA, outBuffSize);
		// mixer_mix(outBuffB, outBuffSize);
		for(int i = 1; i < outBuffSize - 1; i++) {
			outBuffA[i] = (outBuffA[i-1] + outBuffA[i+1])/2;
		}

		// size_t overlap = 1.5;
		// for(int i = 0; i < outBuffSize/overlap; i++) {
			// outBuffA[(outBuffSize/overlap) + i] = (outBuffA[(outBuffSize/overlap) + i] + outBuffB[i])/2;
		// }

		for(int i = 0; i < 1; i++) {
		snd_pcm_sframes_t frames = snd_pcm_writei(handle, outBuffA, outBuffSize);
		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < outBuffSize) {
			printf("Short write (expected %li, wrote %li)\n",
					outBuffSize, frames);
		}
		// size_t overlap = 64;
		// for(int j = 0; j < overlap; j++) {
		// int pointSum = (outBuffA[outBuffSize - overlap + j + 1] + outBuffA[j]) / 2;
		// if(pointSum > SHRT_MAX) {
			// pointSum = SHRT_MAX;
		// } else if (pointSum < SHRT_MIN) {
			// pointSum = SHRT_MIN;
		// }
		// outBuffA[j] = (short)pointSum;
		// }
		}
		// short *tmp = outBuffA;
		// outBuffA = outBuffB;
		// outBuffB = tmp;
	}

	return NULL;
}