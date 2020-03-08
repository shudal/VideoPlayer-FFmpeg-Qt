#include "MyAudio.h"



MyAudio::MyAudio()
{
}


MyAudio::~MyAudio()
{
}

void MyAudio::stop()
{
	if (output) {
		output->stop();
		delete output;
		output = nullptr;
		iod = nullptr;
	}
}

bool MyAudio::start()
{
	stop();

	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channel);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	output = new QAudioOutput(fmt);
	iod = output->start();
	return true;
}

void MyAudio::play(bool isPlay)
{
	if (!output) {
		return;
	}
	if (isPlay) {
		output->resume();
	}
	else {
		output->suspend();
	}
}


bool MyAudio::write(const char* data, int datasize)
{
	if (!data || datasize <= 0) {
		return false;
	}
	if (iod) {
		iod->write(data, datasize);
		return true;
	}
	return false;
	return false;
}

int MyAudio::getFree()
{
	if (!output) {
		return 0;
	}
	int free = output->bytesFree();
	return free; 
}
