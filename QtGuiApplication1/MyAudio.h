#pragma once
#include <QtMultimedia/qaudiooutput.h>
class MyAudio
{
public:
	static MyAudio* GetObj() {
		static MyAudio mau;
		return &mau;
	}
	MyAudio();
	~MyAudio();
	void stop();
	bool start();
	void play(bool);
	bool write(const char*, int);
	int getFree();
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;

	QAudioOutput* output = nullptr;
	QIODevice* iod = nullptr;
};

