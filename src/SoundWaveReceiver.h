#pragma once

extern void (*soundWaveSuccess) (char* data,unsigned int size);

int startSoundWave();
void stopSoundWave();
void soundWaveDataFeed(char* data, unsigned int length);

