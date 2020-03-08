#include "PlayThread.h"
#include "qdebug.h"
#include "QtGuiApplication1.h"

PlayThread::PlayThread(AVFormatContext* afc1, AVFrame* af1, AVFrame* audioframe, int audioSI)
{
	myAfc = afc1;
	myYuv = af1;
	letMeLive = true;
	myPcm = audioframe;
	if (myPcm == NULL) {
		qDebug("status006,1");
	}
	else {
		qDebug("status006,2");
	}
	audioStreamIndex = audioSI;
}


PlayThread::~PlayThread()
{
}

void PlayThread::run() {
	char out[10000] = { 0 }; 
	while (letMeLive) {
		while(apq.size() > 0) {   
			AVPacket pack = apq.front();
			if (myYuv == NULL) {
				myYuv = av_frame_alloc(); 
				qDebug("status%d", 201);
			}
			AVFrame* frame = myYuv;
			int ret = avcodec_send_packet(myAfc->streams[pack.stream_index]->codec, &pack);
			if (ret == 0) { 
				ret = avcodec_receive_frame(myAfc->streams[pack.stream_index]->codec, frame);
				if (ret == 0) {  
				}
				else {  
					qDebug("status105,ret=%d", ret);
				}
			}
			else { 
				qDebug("status104, ret=%d", ret);
			} 
			av_packet_unref(&pack);
			apq.pop();
		}
		if (MyAudio::GetObj()->getFree() < 10000) {
			msleep(1);
			continue;
		}
		AVPacket pack;
		if (myAfc) {
			int err = av_read_frame(myAfc, &pack);
			if (err == 0) {
				if (pack.size > 0) {
					qDebug("status222");
					if (pack.stream_index == audioStreamIndex) { 
						AVFrame* frame = myPcm;
						int ret = avcodec_send_packet(myAfc->streams[pack.stream_index]->codec, &pack);
						qDebug("status005,send packet ok");
						if (ret == 0) {
							qDebug("status005,send audio packet ok"); 
							while ((ret = avcodec_receive_frame(myAfc->streams[pack.stream_index]->codec, frame)) == 0) {
								int len = app->toPCM(out);
								qDebug("status005,len=%d", len);
								MyAudio::GetObj()->write(out, len);
							} 
						}
						else { 
							int ec = 0;
							if (ret == AVERROR(EAGAIN)) { 
								ec = 11;
							}
							else if (ret == AVERROR_EOF) {
								ec = 12;
							}
							else if (ret == AVERROR(EINVAL)) {
								ec = 13;
							}
							else if (ret == AVERROR(ENOMEM)) {
								ec = 14;
							}
							qDebug("status005,2,ret=%d,ec=%d", ret,ec);
						}
						av_packet_unref(&pack);
						continue;
					}
					apq.push(pack);
				}
				else {
					msleep(10);
					qDebug("status102"); 
				} 
			} else {
				qDebug("status101"); 
			}
		} else { 
			qDebug("status103");
		}
	}
}

void PlayThread::myRefresh(AVFormatContext* a, AVFrame* b) {
	myAfc = a;
	myYuv = b;
}