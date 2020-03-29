/* This example reads data from the PCM device
 * and writes to standard wave file for 5 seconds.
 */
#include <stdio.h>
/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <getopt.h>
#include "wave.h"

#define CAPTURE_SEC	5   // record time, second
#define RATE		44100 // sampling frequency
#define SIZE		16
#define CHANNELS	2   // channels
#define RSIZE		64    //buffer size

char *program_name;
void print_usage (FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: %s [ dev... ] \n", program_name);
	fprintf(stream,
		"\t-h  --help           Display this usage information.\n"
		"\t-d  --device         Set device, such as hw:0,1\n"
		"\t-f  --file           Set the file name\n");
	exit(exit_code);
}

int main(int argc, char* argv[])
{
	wave_header wavehead;
	int next_option;
	char *filename = NULL, *device = "default";
	const char *const short_options = "hd:f:";
	const struct option long_options[] = {
		{ "help",   0, NULL, 'h'},
		{ "device", 1, NULL, 'd'},
		{ "file", 1, NULL, 'f'},
		{ NULL,     0, NULL, 0  }
	};


	program_name = argv[0];
	while (1) {
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		if (next_option < 0)
			break;
		switch (next_option) {
		case 'h':
			print_usage (stdout, 0);
			break;
		case 'd':
			device = optarg;
			break;
		case 'f':
			filename = optarg;
			break;
		default:
			abort ();
		}
	}

	if(filename == NULL)
	{
		print_usage(stdout, 1);
	}

	/* fill the wave header */
	memcpy(wavehead.riff_head, "RIFF", 4);
	wavehead.riffdata_len = CAPTURE_SEC * RATE * CHANNELS * SIZE/8 + 36;
	memcpy(wavehead.wave_head, "WAVE", 4);
	memcpy(wavehead.fmt_head, "fmt ", 4);
	wavehead.fmtdata_len = 16;
	wavehead.format_tag = 1;
	wavehead.channels = CHANNELS;
	wavehead.samples_persec = RATE;
	wavehead.bytes_persec = RATE * CHANNELS * SIZE / 8;
	wavehead.block_align = CHANNELS * SIZE / 8;
	wavehead.bits_persec = SIZE;
	memcpy(wavehead.data_head, "data", 4);
	wavehead.data_len = CAPTURE_SEC * RATE * CHANNELS * SIZE/8;
	/* end fill wave header */

	long loops;
	int rc,i = 0;
	int size;
	FILE *fp ;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	unsigned int val,val2;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;
	int wav_fd;

	/* create wave file */
	if(( wav_fd = open(filename, O_CREAT|O_RDWR, 0777)) == -1)
	{
		perror("cannot creat the sound file");
	}

	/* write header section */
	if(write(wav_fd, &wavehead, sizeof(wavehead)) == -1)
	{
		perror("write to sound'head wrong!!");
	}

	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&pcm_handle, device,
			  SND_PCM_STREAM_CAPTURE, 0);
	if (rc < 0)
	{
		fprintf(stderr,  "unable to open pcm device: %s/n",  snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);
	/* Fill it in with default values. */
	snd_pcm_hw_params_any(pcm_handle, params);
	/* Set the desired hardware parameters. */
	/* Interleaved mode */
	snd_pcm_hw_params_set_access(pcm_handle, params,
				     SND_PCM_ACCESS_RW_INTERLEAVED);
	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(pcm_handle, params,
				     SND_PCM_FORMAT_S16_LE);
	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
	/* 44100 bits/second sampling rate (CD quality) */
	val = RATE;
	snd_pcm_hw_params_set_rate_near(pcm_handle, params,  &val, &dir);
	/* Set period size to 32 frames. */
	/* set period size to 32 frames. A frame has 2 samples, left and
	 * right samples. A sample has 2 bytes, most and least significant.
	 */
	frames = RSIZE / (SIZE / 8);
	snd_pcm_hw_params_set_period_size_near(pcm_handle,  params, &frames, &dir);

	/* Write the parameters to the hardware device */
	rc = snd_pcm_hw_params(pcm_handle, params);
	if (rc < 0)
	{
		fprintf(stderr,  "unable to set hw parameters: %s/n",
                snd_strerror(rc));
		exit(1);
	}

	/* display the file info */
	printf("[Display info]\n");
	snd_pcm_hw_params_get_channels(params, &val);
	printf("channels = %d\n", val);
	snd_pcm_hw_params_get_rate(params, &val, &dir);
	printf("rate = %d bps\n", val);
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	printf("period time = %d us\n", val);
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf("period size = %d frames\n", (int)frames);
	snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
	printf("buffer time = %d us\n", val);
	snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);
	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);
	snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
	printf("exact rate = %d/%d bps\n", val, val2);
	val = snd_pcm_hw_params_get_sbits(params);
	printf("significant bits = %d\n", val);
	//snd_pcm_hw_params_get_tick_time(params,  &val, &dir);
	printf("tick time = %d us\n", val);
	val = snd_pcm_hw_params_is_batch(params);
	printf("is batch = %d\n", val);
	val = snd_pcm_hw_params_is_block_transfer(params);
	printf("is block transfer = %d\n", val);
	val = snd_pcm_hw_params_is_double(params);
	printf("is double = %d\n", val);
	val = snd_pcm_hw_params_is_half_duplex(params);
	printf("is half duplex = %d\n", val);
	val = snd_pcm_hw_params_is_joint_duplex(params);
	printf("is joint duplex = %d\n", val);
	val = snd_pcm_hw_params_can_overrange(params);
	printf("can overrange = %d\n", val);
	val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
	printf("can mmap = %d\n", val);
	val = snd_pcm_hw_params_can_pause(params);
	printf("can pause = %d\n", val);
	val = snd_pcm_hw_params_can_resume(params);
	printf("can resume = %d\n", val);
	val = snd_pcm_hw_params_can_sync_start(params);
	printf("can sync start = %d\n", val);
	printf("**************************\n");


	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params,  &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	printf("malloc with size = %d\n",size);
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,  &val, &dir);
	printf("period second: %d\n", val);
	loops = (CAPTURE_SEC * 1000 * 1000) / val;

	//for( i = 0; i < loops; i++)
	for( i = 0; i < (CAPTURE_SEC * RATE * SIZE * CHANNELS / 8)/RSIZE; i++)
	{

		rc = snd_pcm_readi(pcm_handle, buffer, frames);
		//printf("%d\n",i++);
		if (rc == -EPIPE)
		{
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred/n");
			snd_pcm_prepare(pcm_handle);
		} else if (rc < 0) {
			fprintf(stderr, "error from read: %s/n", snd_strerror(rc));
		} else if (rc != (int)frames) {
			fprintf(stderr, "short read, read %d frames/n", rc);
		}

		if(write(wav_fd, buffer, size) == -1)
		{
			perror("write to sound wrong!!");
		}
		//printf("fwrite buffer success\n");
	}

	printf("record done, the file and device will be close\n");
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	close(wav_fd);
	free(buffer);

	return 0;
}
