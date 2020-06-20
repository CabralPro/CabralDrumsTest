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

for(int i = 0; i < 7; i++){
setParamsAlsa(i);
//usleep(1000000);
}

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

snd_pcm_t *pcm[7];
snd_pcm_uframes_t frames;
int buff_size;
char *buff;

// void SetAlsaMasterVolume(long volume)
// {
//     long min, max;
//     snd_mixer_t *handle;
//     snd_mixer_selem_id_t *sid;
//     const char *card = "default";
//     const char *selem_name = "Master";

//     snd_mixer_open(&handle, 0);
//     snd_mixer_attach(handle, card);
//     snd_mixer_selem_register(handle, NULL, NULL);
//     snd_mixer_load(handle);

//     snd_mixer_selem_id_alloca(&sid);
//     snd_mixer_selem_id_set_index(sid, 0);
//     snd_mixer_selem_id_set_name(sid, selem_name);
//     snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

//     snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
// 	 std::cout << "MIN: " << min << std::endl;
// 	 std::cout << "MAX: " << max << std::endl;
// 	 std::cout << "VOL: " << volume * max / 100 << std::endl;

	
//     snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

//     snd_mixer_close(handle);
// }

void setParamsAlsa(int i)
{
	snd_pcm_hw_params_t *params;

	snd_pcm_open(&pcm[i], "default", SND_PCM_STREAM_PLAYBACK, 0);
//snd_pcm_nonblock (pcm[i], 1);

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	/* Fill it with default values */
	snd_pcm_hw_params_any(pcm[i], params);

	/* Set the desired hardware parameters */
	/* Interleaved mode */
	snd_pcm_hw_params_set_access(pcm[i], params,  SND_PCM_ACCESS_RW_INTERLEAVED );
	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(pcm[i], params, SND_PCM_FORMAT_S16_LE);

	/* Single Channel ( 1 : Mono / 2 : Mono) */
	int channels = 1;
	snd_pcm_hw_params_set_channels(pcm[i], params, channels);

	/* sampling rate */
	unsigned int rate = 44100;
	snd_pcm_hw_params_set_rate_near(pcm[i], params, &rate, 0);

	/* Write the parameters to the driver */
	snd_pcm_hw_params(pcm[i], params);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * 2 * channels; // 2 -> sample size  ;;
	buff = (char *)malloc(buff_size);

	//SetAlsaMasterVolume(10000);

}

// play function variables
const char *pad[4] = {"audios/Mono/chimbal.wav", "audios/Mono/bumbo.wav", "audios/Mono/caixa.wav", "audios/Mono/prato.wav"};
int err = 0;

void play(int i, int j)
{
	
int readfd = open(pad[i], O_RDONLY);

	 snd_pcm_prepare(pcm[j]);



	while (read(readfd, buff, buff_size) > 0)
	{

		err = snd_pcm_writei(pcm[j], buff, frames);

		if (err == -EPIPE)
		{
			std::cout << "Xrun" << std::endl;
			int r = snd_pcm_recover(pcm[j], err, 1);
			if(r < 0){
			std::cout << "Xrun 2" << std::endl;

			}
			//snd_pcm_prepare(pcm[3]);
		}else if (err < 0){
			std::cout << "OUTRO ERRO ALEM DO EPIPE" << std::endl;

		}
	}

};