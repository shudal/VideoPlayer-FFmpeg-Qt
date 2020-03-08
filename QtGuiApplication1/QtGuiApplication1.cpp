#include <direct.h>
#include <iostream>
#include <qfiledialog.h>
#include <qlabel.h>  
#include <QtWidgets/QAction> 
#include "QtGuiApplication1.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil\pixfmt.h>
#include <libavutil\imgutils.h>
#include <libavcodec\avcodec.h>
}

using namespace std;
QtGuiApplication1::QtGuiApplication1(QWidget *parent) : QMainWindow(parent) { 
	c1.setupUi(this);
	c1.glWidget->app = this;  
	c1.centralWidget->setLayout(c1.gridLayout); 
	av_register_all();
	myAudio = new MyAudio();

	char path[1000];
	getcwd(path, 1000);
	QString s2 = path; 
	c1.fileNameLabel->setText(s2);
	c1.fileNameLabel->adjustSize(); 
	 
	connect(c1.openFile, &QAction::triggered, [&] { 
		QString s = QFileDialog::getOpenFileName();
		c1.fileNameLabel->setText(s); 
		c1.fileNameLabel->adjustSize();
		if (!s.isEmpty()) {
			myFilePath = s.toStdString();
			myOpenFile(myFilePath.c_str());
		}
	});
}  

void QtGuiApplication1::myOpenFile(const char* path) { 
	int ret = avformat_open_input(&myAfc, path, 0, 0);
	if (ret >= 0) { 
		c1.noticeLabel->setText("file open success");
		bool f = false;
		for (int i = 0; i < myAfc->nb_streams; ++i) {
			qDebug("status004, nbstreams=%d", myAfc->nb_streams);
			AVCodecContext* acc = myAfc->streams[i]->codec;
			AVCodec* codec = avcodec_find_decoder(acc->codec_id);
			if (acc->codec_type == AVMEDIA_TYPE_AUDIO) {
				myAudioStreamIndex = i;
				if (!codec) {
					c1.noticeLabel->setText("unable find decoder");
				} else {
					int aRet = avcodec_open2(acc, codec, NULL);
					if (aRet >= 0) { 
						MyAudio::GetObj()->sampleRate = acc->sample_rate;
						MyAudio::GetObj()->channel = acc->channels;
						switch (acc->sample_fmt)
						{
						case AV_SAMPLE_FMT_S16:
							MyAudio::GetObj()->sampleSize = 16;
							break;
						case AV_SAMPLE_FMT_S32:
							MyAudio::GetObj()->sampleSize = 32;
							break;
						default:
							break;
						}
					} 
				}
			} else if (acc->codec_type == AVMEDIA_TYPE_VIDEO) {
				myVideoStreamIndex = i;
				if (!codec) {
					c1.noticeLabel->setText("unable find decoder");
				}
				else { 
					int aRet = avcodec_open2(acc, codec, NULL);
					if (aRet >= 0) {
						f = true;
					}
				}
			} 
		}
		if (f) { 
			AVCodecContext* acc = myAfc->streams[myVideoStreamIndex]->codec;
			qDebug("status005, vid stream index=%d, audio stream index=%d", myVideoStreamIndex, myAudioStreamIndex);
			AVCodec* codec = avcodec_find_decoder(acc->codec_id);
			c1.noticeLabel->setText("find decoder");
			int err = avcodec_open2(acc, codec, NULL);
			if (err == 0) {
				c1.noticeLabel->setText("open decoder");
				nowVideo = new MyVideo();
				nowVideo->duration = myAfc->duration / 1000000;
				nowVideo->videoFileName = myAfc->filename;
				nowVideo->setSize(acc->width, acc->height);
				qDebug("w=%d h=%d", nowVideo->width, nowVideo->height);
				myYuv = av_frame_alloc();
				nowVideo->myPcm = av_frame_alloc(); 
				if (myPlayThread == nullptr) {
					myPlayThread = new PlayThread(myAfc, myYuv, nowVideo->myPcm, myAudioStreamIndex);
					myPlayThread->app = this;
					myPlayThread->start();
				}
				else {
					myPlayThread = new PlayThread(myAfc, myYuv, nowVideo->myPcm, myAudioStreamIndex);
					myPlayThread->start();
				}
				playVideo = true;

			}
			else {
				c1.noticeLabel->setText("open decoder failed");
			}
		}
		
	}
	else {
		c1.noticeLabel->setText("file not exist");
	}
}


bool QtGuiApplication1::YuvToRGB(char* out, int outWidth, int outHeight) {
	if (this == nullptr) return false;
	if (!myYuv || !myAfc || !playVideo) {
		return false;
	}
	AVCodecContext* videoCtx = myAfc->streams[myVideoStreamIndex]->codec; 
	swsCtx = sws_getCachedContext(swsCtx, videoCtx->width, videoCtx->height,
		videoCtx->pix_fmt,
		outWidth, outHeight,
		AV_PIX_FMT_BGRA,
		SWS_BICUBIC,
		NULL, NULL, NULL
	);
	if (swsCtx) {
		// ok
		uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
		data[0] = (uint8_t *)out;
		int linesize[AV_NUM_DATA_POINTERS] = { 0 };
		linesize[0] = outWidth * 4; // 每一行转码的宽度
		int h = sws_scale(swsCtx, myYuv->data, myYuv->linesize, 0, videoCtx->height,
			data,
			linesize
			);
		return true;
	}
	else {
		// failed;
		qDebug("status%d", 301);
	}
	return false;
}

int QtGuiApplication1::toPCM(char* out)
{
	if (!myAfc || !nowVideo->myPcm || !out) {
		if (!myAfc) {
			return -3;
		}
		if (!nowVideo->myPcm) {
			return -4;
		}
		if (!out) {
			return -5;
		} 
	}
	AVCodecContext* ctx = myAfc->streams[myAudioStreamIndex]->codec;
	if (swrCtx == nullptr) {
		swrCtx = swr_alloc();
		swr_alloc_set_opts(swrCtx, ctx->channel_layout,
			AV_SAMPLE_FMT_S16,
			ctx->sample_rate,
			ctx->channels,
			ctx->sample_fmt,
			ctx->sample_rate,
			0, 0
		);
		swr_init(swrCtx);
	}
	uint8_t* data[1];
	data[0] = (uint8_t*)out;
	int len = swr_convert(swrCtx, data, 10000, (const uint8_t**)nowVideo->myPcm->data, nowVideo->myPcm->nb_samples);
	if (len <= 0) {
		qDebug("status004, len=%d", len);
		return -2;
	}
	int outsize = av_samples_get_buffer_size(NULL, ctx->channels, nowVideo->myPcm->nb_samples, AV_SAMPLE_FMT_S16, 0);
	return outsize;
}
 
 