#include "audio.h"
#include <cassert>
#include <iostream>
#include "utils.h"

std::map<std::string, Audio*> Audio::audiosLoaded;

Audio::Audio()
{
	sample = 0;
}

Audio::~Audio()
{
	BASS_SampleFree(sample);
}

HCHANNEL Audio::playSound(float volume)
{
	if (sample == 0)
		return 0;

	//El handler para un canal
	HCHANNEL hSampleChannel = BASS_SampleGetChannel(sample, false);

	//Lanzamos un sample
	BASS_ChannelPlay(hSampleChannel, true);
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, volume);

	return hSampleChannel;
}

void Audio::changeVolume(HCHANNEL hchannel, float volume)
{
	BASS_ChannelSetAttribute(hchannel, BASS_ATTRIB_VOL, volume);
}

void Audio::StopSound(HCHANNEL channel)
{
	BASS_ChannelStop(channel);
}

HSAMPLE Audio::load(const char* filename)	//HSAMPLE Audio::load()
{
	//stats
	long time = getTime();
	std::cout << " + Audio loading: " << filename << " ... ";

	//El handler para un sample
	HSAMPLE hSample = BASS_SampleLoad(false, filename, 0, 0, 3, 0);

	if (hSample == 0) {
		std::cout << " not found . . .";
	}

	std::cout << " Time: " << (getTime() - time) * 0.001 << "sec" << std::endl;
	return hSample;
}

Audio* Audio::Get(const char* filename)
{
	assert(filename);
	std::map<std::string, Audio*>::iterator it = audiosLoaded.find(filename);
	if (it != audiosLoaded.end()) {
		return it->second;	//it->first = filename, it->second = pointer to the Audio
	}

	//not found . . . time to create it
	Audio* audio = new Audio();
	std::string name = filename;

	HSAMPLE hsample = audio->load(filename);
	
	if (hsample == 0)
	{
		//instead of returning a NULL pointer, return an audio with hsample = 0
		//that way the app will not crash if the audio is not found
		audio->sample = 0;
		return audio;
	}

	audio->sample = hsample;

	//store it in cache
	audiosLoaded[filename] = audio;

	return audio;
}
