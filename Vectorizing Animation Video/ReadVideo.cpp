#include "ReadVideo.h"

#include <auto_link_ffmpeg.hpp>
#include <auto_link_iconv.hpp>

extern "C"
{
	int __image_base__(){return 0;};
};

ReadVideo::ReadVideo(void)
{
	m_HasRead = false;
}


ReadVideo::~ReadVideo(void)
{
	if (m_HasRead)
	{
		av_free(m_buffer);
		av_free(m_pFrameRGB);
		av_free(m_pFrame);
		avcodec_close(m_pCodecCtx);
		avformat_close_input(&m_pFormatCtx);
	}
}

bool ReadVideo::Read(std::string path)
{
	m_pFormatCtx = NULL;
	av_register_all();
	if (avformat_open_input(&m_pFormatCtx, path.c_str(), NULL, NULL) != 0)
	{
		return -1;
	}
	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		return -1;
	}
	av_dump_format(m_pFormatCtx, -1, path.c_str(), 0);
	m_videoStream = -1;
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoStream = i;
			break;
		}
	}
	if (m_videoStream == -1)
	{
		return -1;
	}
	m_pCodecCtx = m_pFormatCtx->streams[m_videoStream]->codec;
	m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	if (m_pCodec == NULL)
	{
		return -1;
	}
	if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0)
	{
		return -1;
	}
	m_pFrame = avcodec_alloc_frame();
	if (m_pFrame == NULL)
	{
		return -1;
	}
	m_pFrameRGB = avcodec_alloc_frame();
	if (m_pFrameRGB == NULL)
	{
		return -1;
	}
	m_numBytes = avpicture_get_size(PIX_FMT_RGB24, m_pCodecCtx->width,
									m_pCodecCtx->height);
	m_buffer = (uint8_t*)av_malloc(m_numBytes);
	avpicture_fill((AVPicture*)m_pFrameRGB, m_buffer, PIX_FMT_RGB24,
				   m_pCodecCtx->width, m_pCodecCtx->height);
}

cv::Mat ReadVideo::GetFrame()
{
	cv::Mat res;
	while (av_read_frame(m_pFormatCtx, &m_packet) >= 0)
	{
		if (m_packet.stream_index == m_videoStream)
		{
			avcodec_decode_video2(m_pCodecCtx, m_pFrame, &m_frameFinished, &m_packet);
			if (m_frameFinished)
			{
				struct SwsContext* img_convert_ctx = NULL;
				img_convert_ctx =
					sws_getCachedContext(img_convert_ctx, m_pCodecCtx->width,
										 m_pCodecCtx->height, m_pCodecCtx->pix_fmt,
										 m_pCodecCtx->width, m_pCodecCtx->height,
										 PIX_FMT_RGB24, SWS_BICUBIC,
										 NULL, NULL, NULL);
				if (!img_convert_ctx)
				{
					fprintf(stderr, "Cannot initialize sws conversion context\n");
					exit(1);
				}
				sws_scale(img_convert_ctx, (const uint8_t * const*)m_pFrame->data,
						  m_pFrame->linesize, 0, m_pCodecCtx->height, m_pFrameRGB->data,
						  m_pFrameRGB->linesize);
				res = _GetFrame(m_pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height);
				av_free_packet(&m_packet);
				return res;
			}
		}
		av_free_packet(&m_packet);
	}
	return cv::Mat();
}

cv::Mat ReadVideo::_GetFrame(AVFrame* pFrame, int width, int height)
{
	cv::Mat res;
	res.create(height, width, CV_8UC3);
	uint8_t* t_data = new uint8_t[width * 3];
	for (int y = 0; y < height; y++)
	{
		memcpy(t_data, pFrame->data[0] + y * pFrame->linesize[0], width * 3);
		for (int i = 0; i < width * 3; i += 3)
		{
			cv::Vec3b& c = res.at<cv::Vec3b>(y, i / 3);
			c[0] = t_data[i + 2];
			c[1] = t_data[i + 1];
			c[2] = t_data[i];
		}
	}
	delete t_data;
	return res;
}
