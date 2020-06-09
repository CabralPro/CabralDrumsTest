#include <curses.h>
#include <iostream>

void setParamsAlsa();
void play(int i);

int main()
{
	// init keyboard mode
	initscr();
	clrtobot();
	keypad(stdscr, TRUE);
	noecho();

	setParamsAlsa();

	char key;

	while ((key = getch()) != 'E')
	{
		switch (toupper(key))
		{
		case 'A':
			play(2); // SNARE DRUM
			break;
		case 'V':
			play(1); // BASS DRUM
			break;
		case 'L':
			play(0); // HI HATS
		}
	}

	endwin(); 
	return 0;
}

// ALSA

#include <alsa/asoundlib.h>

snd_pcm_t *pcm;
snd_pcm_uframes_t frames;
int buff_size;
char *buff;

void setParamsAlsa()
{
	snd_pcm_hw_params_t *params;

	snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	/* Fill it with default values */
	snd_pcm_hw_params_any(pcm, params);

	/* Set the desired hardware parameters */
	/* Interleaved mode */
	snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);

	/* Single Channel ( 1 : mono / 2 : stereo) */
	int channels = 2;
	snd_pcm_hw_params_set_channels(pcm, params, channels);

	/* sampling rate */
	unsigned int rate = 44100;
	snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);

	/* Write the parameters to the driver */
	snd_pcm_hw_params(pcm, params);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2; // 2 -> sample size  ;;
	buff = (char *)malloc(buff_size);
}

// play function variables
const char *pad[3] = {"chimbal.wav", "bumbo.wav", "caixa.wav"};
int err = 0;
int readfd;

void play(int i)
{
	snd_pcm_prepare(pcm);

	readfd = open(pad[i], O_RDONLY);

	while (read(readfd, buff, buff_size) > 0)
	{

		err = snd_pcm_writei(pcm, buff, frames);

		if (err == -EPIPE)
		{
			std::cout << "Reproduction error" << std::endl;
			snd_pcm_recover(pcm, err, 1);
			//snd_pcm_prepare(pcm);
		}
	}
};