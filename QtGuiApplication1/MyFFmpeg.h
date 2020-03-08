#pragma once
class MyFFmpeg
{
public:
	MyFFmpeg();
	~MyFFmpeg();
	bool play = false;
	static MyFFmpeg* GetObj() {
		static MyFFmpeg myFFmpeg;
		return &myFFmpeg;
	}
};

