#include <curses.h>
#include <iostream>

#include <thread>
#include <unistd.h>

void setParamsAlsa(int i);
void play(int i, int j);

int main()
{
	// init keyboard mode
	initscr();
	clrtobot();
	keypad(stdscr, TRUE);
	noecho();

	//std::thread(setParamsAlsa, this);
	//std::thread(setParamsAlsa, this);

setParamsAlsa(0);
setParamsAlsa(1);
setParamsAlsa(2);
setParamsAlsa(3);



//usleep(3000000);


	char key;

	while ((key = toupper(getch())) != 'Q')
	{
		switch (toupper(key))
		{
		case 'A':
			play(2, 0); // SNARE DRUM
			break;
		case 'V':
			play(1, 1); // BASS DRUM
			break;
		case 'L':
			play(0, 2); // HI HATS
			break;
		case 'E':
			play(3, 3); // HI HATS
		}
	}

	endwin(); 
	return 0;
}

// ALSA

#include <alsa/asoundlib.h>

snd_pcm_t *pcm[4];
snd_pcm_uframes_t frames;
int buff_size;
char *buff;

void setParamsAlsa(int i)
{
	snd_pcm_hw_params_t *params;

	snd_pcm_open(&pcm[i], "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);
//snd_pcm_nonblock (pcm[i], 1);

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	/* Fill it with default values */
	snd_pcm_hw_params_any(pcm[i], params);

	/* Set the desired hardware parameters */
	/* Interleaved mode */
	snd_pcm_hw_params_set_access(pcm[i], params,  SND_PCM_ACCESS_RW_NONINTERLEAVED );
	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(pcm[i], params, SND_PCM_FORMAT_S16_LE);

	/* Single Channel ( 1 : mono / 2 : stereo) */
	int channels = 2;
	snd_pcm_hw_params_set_channels(pcm[i], params, channels);

	/* sampling rate */
	unsigned int rate = 44100;
	snd_pcm_hw_params_set_rate_near(pcm[i], params, &rate, 0);

	/* Write the parameters to the driver */
	snd_pcm_hw_params(pcm[i], params);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2; // 2 -> sample size  ;;
	buff = (char *)malloc(buff_size);
}

// play function variables
const char *pad[4] = {"chimbal.wav", "bumbo.wav", "caixa.wav", "prato.wav"};
int err = 0;


void play(int i, int j)
{
	 snd_pcm_prepare(pcm[j]);
int readfd = open(pad[1], O_RDONLY);


	while (read(readfd, buff, buff_size) > 0)
	{

		err = snd_pcm_writei(pcm[j], buff, frames);

		if (err == -EPIPE)
		{
			std::cout << "Reproduction error" << std::endl;
			snd_pcm_recover(pcm[j], err, 1);
			//snd_pcm_prepare(pcm[3]);
		}else if (err < 0){
			std::cout << "OUTRO ERRO ALEM DO EPIPE" << std::endl;

		}
	}

};