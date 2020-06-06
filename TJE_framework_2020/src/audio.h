#ifndef AUDIO_H
#define AUDIO_H

#include "framework.h"
#include <bass.h>
#include <string>
#include <map>

class Audio
{
public:
	static std::map<std::string, Audio*> audiosLoaded;

	HSAMPLE sample; //aqui guardamos el handler del sample que retorna BASS_SampleLoad

	Audio(); //importante poner sample a cero aqui
	~Audio(); //aqui deberiamos liberar el sample con BASS_SampleFree

	HCHANNEL playSound(float volume = 0.5); //lanza el audio y retorna el channel donde suena
	void changeVolume(HCHANNEL hchannel, float volume);
	static void StopSound(HCHANNEL channel); //para parar un audio necesitamos su channel

	HSAMPLE load(const char* filename);
	static Audio* Get(const char* filename); //manager de audios 
};


#endif
