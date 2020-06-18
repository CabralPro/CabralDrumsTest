#include <curses.h>
#include <iostream>

#include <thread>


void setParamsAlsa();
void play(int i);


	template <class T, class Compare>
	inline constexpr const T& clamp( const T& v, const T& lo, const T& hi, Compare comp )
	{
		return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	template <class T>
	inline constexpr const T& clamp( const T& v, const T& lo, const T& hi )
	{
		return clamp( v, lo, hi, std::less<>() );
	}


	template <typename T>
	static void SetThreadPriority(const T& threadHandle, int p, int schedType = SCHED_FIFO)
	{

		if(std::thread::hardware_concurrency() >= 3)
		{
			p = clamp(p, 0, 100);

			auto maxPriority{sched_get_priority_max(SCHED_FIFO)};
			size_t priority = static_cast<size_t>((p * maxPriority) / 100);

			sched_param sch_params;
			sch_params.sched_priority = priority;

			pthread_setschedparam(threadHandle, schedType, &sch_params);
		}
	}

int main()
{
	// init keyboard mode
	initscr();
	clrtobot();
	keypad(stdscr, TRUE);
	noecho();

	setParamsAlsa();

	char key;


	// std::thread playThread;
	// playThread = std::thread(play, 2);
	// SetThreadPriority(playThread.native_handle(), 100);

	std::thread playThread;


	while ((key = getch()) != 'E')
	{
		switch (toupper(key))
		{
		case 'A':
			// play(2); // SNARE DRUM
{

	playThread = std::thread(play, 2);
	SetThreadPriority(playThread.native_handle(), 100);
	playThread.join();
}
			break;
		case 'V':
			// play(1); // BASS DRUM

			{


	playThread = std::thread(play, 1);
	SetThreadPriority(playThread.native_handle(), 100);
	playThread.join();
}

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
	snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
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



//software
	snd_pcm_sw_params_t *sw_params;

snd_pcm_sw_params_malloc (&sw_params);
snd_pcm_sw_params_current (pcm, sw_params);
snd_pcm_sw_params_set_start_threshold(pcm, sw_params, buff_size  - frames);
snd_pcm_sw_params_set_avail_min(pcm, sw_params, frames);
snd_pcm_sw_params(pcm, sw_params);





	snd_pcm_prepare(pcm);

}

// play function variables
const char *pad[3] = {"chimbal.wav", "bumbo.wav", "caixa.wav"};
int err = 0;
int readfd;

void play(int i)
{
	//snd_pcm_prepare(pcm);

	readfd = open(pad[i], O_NONBLOCK);
	std::cout << readfd << std::endl;

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


	std::cout << "FIM" << std::endl;

};



