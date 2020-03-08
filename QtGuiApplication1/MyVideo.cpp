#include "MyVideo.h"



MyVideo::MyVideo()
{
}


MyVideo::~MyVideo()
{
}
void MyVideo::setSize(int x, int y) {
	width = x;
	height = y;
	hdivw = 1.0*x / y;
}
