#pragma once
#include "qopenglwidget.h"  
#include "qimage.h"

class QtGuiApplication1;
using namespace std;
class MyGLWidget: public QOpenGLWidget
{ 
public:
	MyGLWidget(QWidget *);
	~MyGLWidget(); 
	QtGuiApplication1 * app = nullptr;
	QSize* mysize = nullptr;
	void paintEvent(QPaintEvent*);
	QImage* image = nullptr;
	QImage* coverImage = nullptr; 
	//void resizeEvent(QResizeEvent* event);
	void resizeGL(int w, int h);
};

