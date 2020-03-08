#pragma once
#include "qthread.h"
#include <queue>
extern "C" {
#include <libavformat/avformat.h>
}
using namespace std;

class QtGuiApplication1;
class PlayThread :
	public QThread
{ 
public:
	void run();
	queue<AVPacket> apq;
	AVFrame* myYuv;
	AVFormatContext* myAfc;
	AVFrame* myPcm;
	int audioStreamIndex = 0;
	PlayThread(AVFormatContext*, AVFrame*, AVFrame*, int);
	~PlayThread(); 
	bool letMeLive = true;
	void myRefresh(AVFormatContext*, AVFrame*);
	QtGuiApplication1* app = nullptr;
};

