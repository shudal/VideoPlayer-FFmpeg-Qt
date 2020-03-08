#pragma once
#include <string>
extern "C" {
#include <libavformat/avformat.h>
}
using namespace std;
class MyVideo
{
public:
	MyVideo();
	~MyVideo();
	int duration;
	string videoFileName;
	int width, height;
	double hdivw;
	void setSize(int, int);
	AVFrame* myPcm = nullptr;
};

