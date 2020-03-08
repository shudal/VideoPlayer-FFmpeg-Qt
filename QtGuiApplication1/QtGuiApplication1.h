#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h" 
#include "PlayThread.h"
#include <string>
#include "MyVideo.h"
#include "MyAudio.h"
extern "C" {
#include <libavformat/avformat.h>
}
using namespace std;
class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	Ui_QtGuiApplication1Class c1;
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);
	string myFilePath;
	void myOpenFile(const char*);
	int myVideoStreamIndex=100;
	int myAudioStreamIndex=99;
	AVFormatContext* myAfc = NULL;
	struct SwsContext* swsCtx = nullptr;
	struct SwrContext* swrCtx = nullptr;
	int outWidth, outHeight;
	AVFrame* myYuv = nullptr;
	bool YuvToRGB(char*, int, int); 
	PlayThread* myPlayThread=nullptr; 
	bool playVideo = false;
	bool SizeChanged = false;
	MyVideo* nowVideo = nullptr;
	int toPCM(char*);
	MyAudio* myAudio = nullptr;
private:
	Ui::QtGuiApplication1Class ui;
};
