#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
};

class ReadVideo
{
public:
	ReadVideo(void);
	~ReadVideo(void);
	bool Read(std::string path);
	cv::Mat GetFrame();
	void SkipFrame();
private:
	cv::Mat _GetFrame(AVFrame *pFrame, int width, int height);
	bool	m_HasRead;
	std::string m_Path;
	AVFormatContext *m_pFormatCtx;
	int             m_videoStream;
	AVCodecContext  *m_pCodecCtx;
	AVCodec         *m_pCodec;
	AVFrame         *m_pFrame;
	AVFrame         *m_pFrameRGB;
	AVPacket        m_packet;
	int             m_frameFinished;
	int             m_numBytes;
	uint8_t         *m_buffer;
};

