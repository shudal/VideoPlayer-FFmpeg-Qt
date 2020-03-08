#include "MyGLWidget.h"
#include "qpainter.h" 
#include "QtGuiApplication1.h"
#include <qtimer.h>
#include "qdebug.h"
#include "MyAudio.h"

using namespace std;
MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{ 
	coverImage = new QImage();
	coverImage->load("./Resources/img/DefaultVideoCover.jpg");   

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(20);
}


MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::paintEvent(QPaintEvent* e) {
	QPainter painter;
	painter.begin(this);   
	if (!app->playVideo) {

		coverImage->load("./Resources/img/DefaultVideoCover.jpg");
		painter.drawImage(QPoint(0, 0), *coverImage);
		return;
	}
	if (image == nullptr) {
		qDebug("status101"); 
		int h = width() * app->nowVideo->hdivw;
		int w = width();
		uchar *buf = new uchar[h * w * 4]; 
		image = new QImage(buf, w, h, QImage::Format_ARGB32); 
		MyAudio::GetObj()->start(); 
		MyAudio::GetObj()->play(true);
	}  
	bool ret = app->YuvToRGB((char*)(image->bits()), width(), height());
	//bool ret = app->YuvToRGB((char*)(vidImg->bits()), width(), height());
	painter.drawImage(QPoint(0,0), *image);
}
void MyGLWidget::resizeGL(int w, int h) { 
	coverImage->load("./Resources/img/DefaultVideoCover.jpg");
	qDebug("myglwidget resize"); 
	if (app->nowVideo != nullptr) {
		int h = width() * app->nowVideo->hdivw;
		int w = width();
		uchar *buf = new uchar[h * w * 4];
		if (image != nullptr) { 
			delete image;
		}
		image = new QImage(buf, w, h, QImage::Format_ARGB32);
	}
	coverImage->load("./Resources/img/DefaultVideoCover.jpg");
}