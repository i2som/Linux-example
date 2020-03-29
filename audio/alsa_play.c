#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <getopt.h>
#include "wave.h"

int set_pcm_play(FILE *fp, char *device);

wave_header wav_head;
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


int main(int argc,char *argv[])
{
	int nread, next_option;
	FILE *fp;
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

	fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		perror("open file failed:\n");
		exit(1);
	}

	nread = fread(&wav_head, 1, sizeof(wav_head), fp);
	printf("nread=%d\n", nread);

	printf("File name: %s\n", filename);
	printf("File size: %d\n", wav_head.riffdata_len);

	printf("Number of Channels: %d\n", wav_head.channels);
	printf("Sample Rate: %d\n", wav_head.samples_persec);
	printf("Bits per sample: %d\n", wav_head.bits_persec);

	printf("Size of data section: %d\n", wav_head.data_len);

	set_pcm_play(fp, device);
	return 0;
}

int set_pcm_play(FILE *fp, char *device)
{
	int rc;
	int ret;
	int size;
	snd_pcm_t* handle; //PCM device fd
	snd_pcm_hw_params_t* params;
	unsigned int val;
	int dir=0;
	snd_pcm_uframes_t frames;
	char *buffer;
	int channels = wav_head.channels;
	int frequency = wav_head.samples_persec;
	int bit = wav_head.bits_persec;
	int datablock = wav_head.block_align;
	unsigned char ch[100]; // store data of wav header

	rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if(rc < 0)
	{
		perror("\nopen PCM device failed:");
		exit(1);
	}

	snd_pcm_hw_params_alloca(&params);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_alloca:");
		exit(1);
	}

	/* init the params */
	rc = snd_pcm_hw_params_any(handle, params);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_any:");
		exit(1);
	}

	/* init the access permission */
	rc = snd_pcm_hw_params_set_access(handle, params,
					  SND_PCM_ACCESS_RW_INTERLEAVED);
        if(rc<0)
        {
                perror("\nsed_pcm_hw_set_access:");
                exit(1);
        }

	// sampling bit
        switch(bit/8)
	{
	case 1:snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U8);
	       break ;
	case 2:snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
	       break ;
	case 3:snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S24_LE);
	       break ;
	}

	// setting channels, 1 is single channel, 2 is strero
	rc = snd_pcm_hw_params_set_channels(handle, params, channels);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_set_channels:");
		exit(1);
	}

	val = frequency;
	// setting frequency
	rc = snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
	if(rc < 0)
	{
		perror("\nsnd_pcm_hw_params_set_rate_near:");
		exit(1);
	}

	rc = snd_pcm_hw_params(handle, params);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params: ");
		exit(1);
	}

	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	if(rc<0)
	{
		perror("\nsnd_pcm_hw_params_get_period_size:");
		exit(1);
	}

	/* data block size */
	size = frames * datablock;

	buffer =(char*)malloc(size);
	// seek into data section
	fseek(fp,58,SEEK_SET);

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		ret = fread(buffer, 1, size, fp);
		if(ret == 0)
		{
			printf("Load file successful\n");
			break;
		} else if (ret != size)
		{
		}
		// write audio data into PCM device
		while(ret = snd_pcm_writei(handle, buffer, frames)<0)
		{
			usleep(2000);
			if (ret == -EPIPE)
			{
				/* EPIPE means underrun */
				fprintf(stderr, "underrun occurred\n");
				// re-setup device
				snd_pcm_prepare(handle);
			} else if (ret < 0) {
				fprintf(stderr, "error from writei: %s\n",
					snd_strerror(ret));
			}
		}
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	return 0;
}
