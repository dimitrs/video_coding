/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef cap__ffmpeg_impl_h
#define cap__ffmpeg_impl_h

#define _CRT_SECURE_NO_WARNINGS

//#define LIBAVFORMAT_BUILD 10000000
#define snprintf _snprintf
#include <stdio.h>
#include <assert.h>

#if defined _MSC_VER && _MSC_VER >= 1200
#pragma warning( disable: 4244 4510 4512 4610 4146 4996 4005)

#define sprintf sprintf_s
#define _sprintf _sprintf_s
#define _snprintf _snprintf_s

#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(WIN32) || defined(__MINGW32__)
// some versions of FFMPEG assume a C99 compiler, and don't define INT64_C
#if !defined INT64_C || !defined UINT64_C
#define INT64_C
#define UINT64_C
#define __STDC_CONSTANT_MACROS
// force re-inclusion of stdint.h to get INT64_C macro
#undef _STDINT_H
#include <stdint.h>
#endif
#include <errno.h>
#endif

#ifdef WIN32
  #define HAVE_FFMPEG_SWSCALE 1
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
#else

// if the header path is not specified explicitly, let's deduce it
#if !defined HAVE_FFMPEG_AVCODEC_H && !defined HAVE_LIBAVCODEC_AVCODEC_H

#if defined(HAVE_GENTOO_FFMPEG)
  #define HAVE_LIBAVCODEC_AVCODEC_H 1
  #define HAVE_LIBAVFORMAT_AVFORMAT_H 1
  #if defined(HAVE_FFMPEG_SWSCALE)
    #define HAVE_LIBSWSCALE_SWSCALE_H 1
  #endif
#elif defined HAVE_FFMPEG
  #define HAVE_FFMPEG_AVCODEC_H 1
  #define HAVE_FFMPEG_AVFORMAT_H 1
  #if defined(HAVE_FFMPEG_SWSCALE)
    #define HAVE_FFMPEG_SWSCALE_H 1
  #endif
#endif

#endif

#if defined(HAVE_FFMPEG_AVCODEC_H)
  #include <ffmpeg/avcodec.h>
#endif
#if defined(HAVE_FFMPEG_AVFORMAT_H)
  #include <ffmpeg/avformat.h>
#endif
#if defined(HAVE_FFMPEG_SWSCALE_H)
  #include <ffmpeg/swscale.h>
#endif

#if defined(HAVE_LIBAVFORMAT_AVFORMAT_H)
  #include <libavformat/avformat.h>
#endif
#if defined(HAVE_LIBAVCODEC_AVCODEC_H)
  #include <libavcodec/avcodec.h>
#endif
#if defined(HAVE_LIBSWSCALE_SWSCALE_H)
  #include <libswscale/swscale.h>
#endif

#endif

#ifdef __cplusplus
}
#endif

#if defined _MSC_VER && _MSC_VER >= 1200
#pragma warning( default: 4244 4510 4512 4610 )
#endif

#ifdef NDEBUG
#define CV_WARN(message)
#else
#define CV_WARN(message) fprintf(stderr, "warning: %s (%s:%d)\n", message, __FILE__, __LINE__)
#endif


#ifndef MKTAG
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
#endif

/* PIX_FMT_RGBA32 macro changed in newer ffmpeg versions */
#ifndef PIX_FMT_RGBA32
#define PIX_FMT_RGBA32 PIX_FMT_RGB32
#endif

#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )



#if defined WIN32 || defined _WIN32
    #include <windows.h>
#elif defined __linux__ || defined __APPLE__
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/types.h> 
    #include <sys/sysctl.h>
#endif

int get_number_of_cpus(void)
{
#if defined WIN32 || defined _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    
    return (int)sysinfo.dwNumberOfProcessors;
#elif defined __linux__
    return (int)sysconf( _SC_NPROCESSORS_ONLN );
#elif defined __APPLE__
    int numCPU=0;
    int mib[4];
    size_t len = sizeof(numCPU); 
    
    /* set the mib for hw.ncpu */
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;
    
    /* get the number of CPUs from the system */
    sysctl(mib, 2, &numCPU, &len, NULL, 0);
    
    if( numCPU < 1 ) 
    {
        mib[1] = HW_NCPU;
        sysctl( mib, 2, &numCPU, &len, NULL, 0 );
        
        if( numCPU < 1 )
            numCPU = 1;
    }

    return (int)numCPU;
#else
    return 1;
#endif
}


char * FOURCC2str( int fourcc )
{
    char * mystr=(char*)malloc(5);
    mystr[0]=(char)((fourcc    )&255);
    mystr[1]=(char)((fourcc>> 8)&255);
    mystr[2]=(char)((fourcc>>16)&255);
    mystr[3]=(char)((fourcc>>24)&255);
    mystr[4]=0;
    return mystr;
}


// required to look up the correct codec ID depending on the FOURCC code,
// this is just a snipped from the file riff.c from ffmpeg/libavformat
typedef struct AVCodecTag {
    int id;
    unsigned int tag;
} AVCodecTag;

const AVCodecTag codec_bmp_tags[] = {
    { CODEC_ID_H264, MKTAG('H', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('h', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('X', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('x', '2', '6', '4') },
    { CODEC_ID_H264, MKTAG('a', 'v', 'c', '1') },
    { CODEC_ID_H264, MKTAG('V', 'S', 'S', 'H') },

    { CODEC_ID_H263, MKTAG('H', '2', '6', '3') },
    { CODEC_ID_H263P, MKTAG('H', '2', '6', '3') },
    { CODEC_ID_H263I, MKTAG('I', '2', '6', '3') }, /* intel h263 */
    { CODEC_ID_H261, MKTAG('H', '2', '6', '1') },

    /* added based on MPlayer */
    { CODEC_ID_H263P, MKTAG('U', '2', '6', '3') },
    { CODEC_ID_H263P, MKTAG('v', 'i', 'v', '1') },

    { CODEC_ID_MPEG4, MKTAG('F', 'M', 'P', '4') },
    { CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', 'X') },
    { CODEC_ID_MPEG4, MKTAG('D', 'X', '5', '0') },
    { CODEC_ID_MPEG4, MKTAG('X', 'V', 'I', 'D') },
    { CODEC_ID_MPEG4, MKTAG('M', 'P', '4', 'S') },
    { CODEC_ID_MPEG4, MKTAG('M', '4', 'S', '2') },
    { CODEC_ID_MPEG4, MKTAG(0x04, 0, 0, 0) }, /* some broken avi use this */

    /* added based on MPlayer */
    { CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', '1') },
    { CODEC_ID_MPEG4, MKTAG('B', 'L', 'Z', '0') },
    { CODEC_ID_MPEG4, MKTAG('m', 'p', '4', 'v') },
    { CODEC_ID_MPEG4, MKTAG('U', 'M', 'P', '4') },
    { CODEC_ID_MPEG4, MKTAG('W', 'V', '1', 'F') },
    { CODEC_ID_MPEG4, MKTAG('S', 'E', 'D', 'G') },

    { CODEC_ID_MPEG4, MKTAG('R', 'M', 'P', '4') },

    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '3') }, /* default signature when using MSMPEG4 */
    { CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', '4', '3') },

    /* added based on MPlayer */
    { CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', 'G', '3') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '5') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '6') },
    { CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '4') },
    { CODEC_ID_MSMPEG4V3, MKTAG('A', 'P', '4', '1') },
    { CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '1') },
    { CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '0') },

    { CODEC_ID_MSMPEG4V2, MKTAG('M', 'P', '4', '2') },

    /* added based on MPlayer */
    { CODEC_ID_MSMPEG4V2, MKTAG('D', 'I', 'V', '2') },

    { CODEC_ID_MSMPEG4V1, MKTAG('M', 'P', 'G', '4') },

    { CODEC_ID_WMV1, MKTAG('W', 'M', 'V', '1') },

    /* added based on MPlayer */
    { CODEC_ID_WMV2, MKTAG('W', 'M', 'V', '2') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'd') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'h', 'd') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'l') },
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', '2', '5') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '1') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '2') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'p', 'g', '2') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('M', 'P', 'E', 'G') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('P', 'I', 'M', '1') },
    { CODEC_ID_MPEG1VIDEO, MKTAG('V', 'C', 'R', '2') },
    { CODEC_ID_MPEG1VIDEO, 0x10000001 },
    { CODEC_ID_MPEG2VIDEO, 0x10000002 },
    { CODEC_ID_MPEG2VIDEO, MKTAG('D', 'V', 'R', ' ') },
    { CODEC_ID_MPEG2VIDEO, MKTAG('M', 'M', 'E', 'S') },
    { CODEC_ID_MJPEG, MKTAG('M', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('L', 'J', 'P', 'G') },
    { CODEC_ID_LJPEG, MKTAG('L', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('J', 'P', 'G', 'L') }, /* Pegasus lossless JPEG */
    { CODEC_ID_MJPEG, MKTAG('M', 'J', 'L', 'S') }, /* JPEG-LS custom FOURCC for avi - decoder */
    { CODEC_ID_MJPEG, MKTAG('j', 'p', 'e', 'g') },
    { CODEC_ID_MJPEG, MKTAG('I', 'J', 'P', 'G') },
    { CODEC_ID_MJPEG, MKTAG('A', 'V', 'R', 'n') },
    { CODEC_ID_HUFFYUV, MKTAG('H', 'F', 'Y', 'U') },
    { CODEC_ID_FFVHUFF, MKTAG('F', 'F', 'V', 'H') },
    { CODEC_ID_CYUV, MKTAG('C', 'Y', 'U', 'V') },
    { CODEC_ID_RAWVIDEO, 0 },
    { CODEC_ID_RAWVIDEO, MKTAG('I', '4', '2', '0') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', 'U', 'Y', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', '4', '2', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', 'V', '1', '2') },
    { CODEC_ID_RAWVIDEO, MKTAG('U', 'Y', 'V', 'Y') },
    { CODEC_ID_RAWVIDEO, MKTAG('I', 'Y', 'U', 'V') },
    { CODEC_ID_RAWVIDEO, MKTAG('Y', '8', '0', '0') },
    { CODEC_ID_RAWVIDEO, MKTAG('H', 'D', 'Y', 'C') },
    { CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '1') },
    { CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '2') },
    { CODEC_ID_VP3, MKTAG('V', 'P', '3', '1') },
    { CODEC_ID_VP3, MKTAG('V', 'P', '3', '0') },
    { CODEC_ID_ASV1, MKTAG('A', 'S', 'V', '1') },
    { CODEC_ID_ASV2, MKTAG('A', 'S', 'V', '2') },
    { CODEC_ID_VCR1, MKTAG('V', 'C', 'R', '1') },
    { CODEC_ID_FFV1, MKTAG('F', 'F', 'V', '1') },
    { CODEC_ID_XAN_WC4, MKTAG('X', 'x', 'a', 'n') },
    { CODEC_ID_MSRLE, MKTAG('m', 'r', 'l', 'e') },
    { CODEC_ID_MSRLE, MKTAG(0x1, 0x0, 0x0, 0x0) },
    { CODEC_ID_MSVIDEO1, MKTAG('M', 'S', 'V', 'C') },
    { CODEC_ID_MSVIDEO1, MKTAG('m', 's', 'v', 'c') },
    { CODEC_ID_MSVIDEO1, MKTAG('C', 'R', 'A', 'M') },
    { CODEC_ID_MSVIDEO1, MKTAG('c', 'r', 'a', 'm') },
    { CODEC_ID_MSVIDEO1, MKTAG('W', 'H', 'A', 'M') },
    { CODEC_ID_MSVIDEO1, MKTAG('w', 'h', 'a', 'm') },
    { CODEC_ID_CINEPAK, MKTAG('c', 'v', 'i', 'd') },
    { CODEC_ID_TRUEMOTION1, MKTAG('D', 'U', 'C', 'K') },
    { CODEC_ID_MSZH, MKTAG('M', 'S', 'Z', 'H') },
    { CODEC_ID_ZLIB, MKTAG('Z', 'L', 'I', 'B') },
    { CODEC_ID_SNOW, MKTAG('S', 'N', 'O', 'W') },
    { CODEC_ID_4XM, MKTAG('4', 'X', 'M', 'V') },
    { CODEC_ID_FLV1, MKTAG('F', 'L', 'V', '1') },
    { CODEC_ID_SVQ1, MKTAG('s', 'v', 'q', '1') },
    { CODEC_ID_TSCC, MKTAG('t', 's', 'c', 'c') },
    { CODEC_ID_ULTI, MKTAG('U', 'L', 'T', 'I') },
    { CODEC_ID_VIXL, MKTAG('V', 'I', 'X', 'L') },
    { CODEC_ID_QPEG, MKTAG('Q', 'P', 'E', 'G') },
    { CODEC_ID_QPEG, MKTAG('Q', '1', '.', '0') },
    { CODEC_ID_QPEG, MKTAG('Q', '1', '.', '1') },
    { CODEC_ID_WMV3, MKTAG('W', 'M', 'V', '3') },
    { CODEC_ID_LOCO, MKTAG('L', 'O', 'C', 'O') },
    { CODEC_ID_THEORA, MKTAG('t', 'h', 'e', 'o') },
#if LIBAVCODEC_VERSION_INT>0x000409
    { CODEC_ID_WNV1, MKTAG('W', 'N', 'V', '1') },
    { CODEC_ID_AASC, MKTAG('A', 'A', 'S', 'C') },
    { CODEC_ID_INDEO2, MKTAG('R', 'T', '2', '1') },
    { CODEC_ID_FRAPS, MKTAG('F', 'P', 'S', '1') },
    { CODEC_ID_TRUEMOTION2, MKTAG('T', 'M', '2', '0') },
#endif
#if LIBAVCODEC_VERSION_INT>((50<<16)+(1<<8)+0)
    { CODEC_ID_FLASHSV, MKTAG('F', 'S', 'V', '1') },
    { CODEC_ID_JPEGLS,MKTAG('M', 'J', 'L', 'S') }, /* JPEG-LS custom FOURCC for avi - encoder */
    { CODEC_ID_VC1, MKTAG('W', 'V', 'C', '1') },
    { CODEC_ID_VC1, MKTAG('W', 'M', 'V', 'A') },
    { CODEC_ID_CSCD, MKTAG('C', 'S', 'C', 'D') },
    { CODEC_ID_ZMBV, MKTAG('Z', 'M', 'B', 'V') },
    { CODEC_ID_KMVC, MKTAG('K', 'M', 'V', 'C') },
#endif
#if LIBAVCODEC_VERSION_INT>((51<<16)+(11<<8)+0)
    { CODEC_ID_VP5, MKTAG('V', 'P', '5', '0') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '0') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '1') },
    { CODEC_ID_VP6, MKTAG('V', 'P', '6', '2') },
    { CODEC_ID_VP6F, MKTAG('V', 'P', '6', 'F') },
    { CODEC_ID_JPEG2000, MKTAG('M', 'J', '2', 'C') },
    { CODEC_ID_VMNC, MKTAG('V', 'M', 'n', 'c') },
#endif
#if LIBAVCODEC_VERSION_INT>=((51<<16)+(49<<8)+0)
// this tag seems not to exist in older versions of FFMPEG
    { CODEC_ID_TARGA, MKTAG('t', 'g', 'a', ' ') },
#endif
    { CODEC_ID_NONE, 0 },
};


struct Image_FFMPEG
{
    unsigned char* data;
    int step;
    int width;
    int height;
    int cn;
};


inline void _opencv_ffmpeg_free(void** ptr)
{
    if(*ptr) free(*ptr);
    *ptr = 0;
}


struct CvCapture_FFMPEG
{
    bool open(int width, int height);
    void close();

    IplImage* retrieveFrame();
    bool grabFrame(uint8_t * data, int length);

    void init();

    AVFrame *alloc_pictureBGR24(int width, int height);

	AVPacket avpkt;
	AVCodec * codec;
	AVCodecContext *c;
	AVFrame* picture;
    char buf[1024*30];
	IplImage* dst2;
    int key_frame;
    uint8_t *buffer2;
    int width;
    int height;
};


void CvCapture_FFMPEG::init()
{
}

void CvCapture_FFMPEG::close()
{
    if( picture )
    av_free(picture);

    // free last packet if exist
    if (avpkt.data) {
        av_free_packet (&avpkt);
    }
}


#ifndef AVSEEK_FLAG_FRAME
	#define AVSEEK_FLAG_FRAME 0
#endif

bool CvCapture_FFMPEG::open(int width, int height)
{
	dst2 = NULL;
    buffer2 = NULL;

	avcodec_init();
	avcodec_register_all();
	av_init_packet(&avpkt);

    codec = avcodec_find_decoder(CODEC_ID_MPEG4);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	c = avcodec_alloc_context3(codec);
	picture= avcodec_alloc_frame();

	if(codec->capabilities&CODEC_CAP_TRUNCATED)
		c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

	c->width = width;
	c->height = height;
	c->channels = 1;

	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	MUST be initialized there because this information is not
	available in the bitstream. */

	// open it 
	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		return false;
	}

    return true;
}


IplImage* CvCapture_FFMPEG::retrieveFrame()
{
    return dst2;
}

bool CvCapture_FFMPEG::grabFrame(uint8_t * data, int length)
{
    if (dst2)
    {
        cvReleaseImage(&dst2);
    }

	int frame = 0;
	avpkt.size = length;
    if (avpkt.size == 0)
        return false;

    avpkt.data = data;
    while (avpkt.size > 0) {
        int got_picture;
        int len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);
        if (len < 0) {
            fprintf(stderr, "Error while decoding frame %d\n", frame);
            dst2 = NULL;
            return false;
        }
        key_frame = picture->key_frame;
        if (got_picture) {
			AVFrame* rgb_picture = alloc_pictureBGR24(picture->width, picture->height) ;
			struct SwsContext * img_convert_ctx = sws_getContext(
			    picture->width, picture->height, PIX_FMT_YUV420P,
				picture->width, picture->height, PIX_FMT_BGR24,
				SWS_BICUBIC,
				NULL, NULL, NULL);

            sws_scale(
			    img_convert_ctx, picture->data, picture->linesize, 0, picture->height,
				rgb_picture->data, rgb_picture->linesize);

            sws_freeContext(img_convert_ctx);

			dst2 = cvCreateImage( cvSize(picture->width, picture->height), IPL_DEPTH_8U, 3);
			for (int i = 0; i < dst2->imageSize; i++) {
			    dst2->imageData[i] = rgb_picture->data[0][i];
            }
            delete [] buffer2;
            av_free(rgb_picture);
        }
        avpkt.size -= len;
        avpkt.data += len;
    }
    return true;
}

#if defined(__APPLE__)
#define AV_NOPTS_VALUE_ ((int64_t)0x8000000000000000LL)
#else
#define AV_NOPTS_VALUE_ ((int64_t)AV_NOPTS_VALUE)
#endif


AVFrame *CvCapture_FFMPEG::alloc_pictureBGR24(int width, int height) 
{
    AVFrame *pFrameRGB = avcodec_alloc_frame();
	if(pFrameRGB==NULL) return NULL;
	int numBytes = avpicture_get_size(PIX_FMT_BGR24, width, height);
	buffer2=new uint8_t[numBytes];
	avpicture_fill((AVPicture *)pFrameRGB, buffer2, PIX_FMT_BGR24, width, height); 
	return pFrameRGB;
}






///////////////// FFMPEG CvVideoWriter implementation //////////////////////////
struct CvVideoWriter_FFMPEG
{
    bool open( const char* filename, int fourcc,
        double fps, int width, int height, bool isColor );
    void close();
    bool writeFrame( const unsigned char* data, int step, int width, int height, int cn, int origin );

    void init();

    AVOutputFormat *fmt;
    AVFormatContext *oc;
    uint8_t         * outbuf;
    uint32_t          outbuf_size;
    FILE            * outfile;
    AVFrame         * picture;
    AVFrame         * input_picture;
    uint8_t         * picbuf;
    AVStream        * video_st;
    int               input_pix_fmt;
    Image_FFMPEG      temp_image;
#if defined(HAVE_FFMPEG_SWSCALE)
    struct SwsContext *img_convert_ctx;
#endif
	uint32_t	out_size;
};

static const char * icvFFMPEGErrStr(int err)
{
#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(53, 4, 0)
    switch(err) {
 		case AVERROR_BSF_NOT_FOUND:
 			return "Bitstream filter not found";
 		case AVERROR_DECODER_NOT_FOUND:
 			return "Decoder not found";
 		case AVERROR_DEMUXER_NOT_FOUND:
 			return "Demuxer not found";
 		case AVERROR_ENCODER_NOT_FOUND:
 			return "Encoder not found";
 		case AVERROR_EOF:
 			return "End of file";
 		case AVERROR_EXIT:
 			return "Immediate exit was requested; the called function should not be restarted";
 		case AVERROR_FILTER_NOT_FOUND:
 			return "Filter not found";
 		case AVERROR_INVALIDDATA:
 			return "Invalid data found when processing input";
 		case AVERROR_MUXER_NOT_FOUND:
 			return "Muxer not found";
 		case AVERROR_OPTION_NOT_FOUND:
 			return "Option not found";
 		case AVERROR_PATCHWELCOME:
 			return "Not yet implemented in FFmpeg, patches welcome";
 		case AVERROR_PROTOCOL_NOT_FOUND:
 			return "Protocol not found";
 		case AVERROR_STREAM_NOT_FOUND:
 			return "Stream not found";
 		default:
 			break;
 	}
#else
    switch(err) {
    case AVERROR_NUMEXPECTED:
		return "Incorrect filename syntax";
    case AVERROR_INVALIDDATA:
		return "Invalid data in header";
    case AVERROR_NOFMT:
		return "Unknown format";
    case AVERROR_IO:
		return "I/O error occurred";
    case AVERROR_NOMEM:
		return "Memory allocation error";
    default:
		break;
    }
#endif

 	return "Unspecified error";
}

/* function internal to FFMPEG (libavformat/riff.c) to lookup codec id by fourcc tag*/
extern "C" {
	enum CodecID codec_get_bmp_id(unsigned int tag);
}

void CvVideoWriter_FFMPEG::init()
{
    fmt = 0;
    oc = 0;
    outbuf = 0;
    outbuf_size = 0;
    outfile = 0;
    picture = 0;
    input_picture = 0;
    picbuf = 0;
    video_st = 0;
    input_pix_fmt = 0;
    memset(&temp_image, 0, sizeof(temp_image));
#if defined(HAVE_FFMPEG_SWSCALE)
    img_convert_ctx = 0;
#endif
	out_size=0;
}

/**
 * the following function is a modified version of code
 * found in ffmpeg-0.4.9-pre1/output_example.c
 */
static AVFrame * icv_alloc_picture_FFMPEG(int pix_fmt, int width, int height, bool alloc)
{
	AVFrame * picture;
	uint8_t * picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size( (PixelFormat) pix_fmt, width, height);
	if(alloc){
		picture_buf = (uint8_t *) malloc(size);
		if (!picture_buf)
		{
			av_free(picture);
			return NULL;
		}
		avpicture_fill((AVPicture *)picture, picture_buf,
				(PixelFormat) pix_fmt, width, height);
	}
	else {
	}
	return picture;
}

/* add a video output stream to the container */
static AVStream *icv_add_video_stream_FFMPEG(AVFormatContext *oc,
		                                     CodecID codec_id,
											 int w, int h, int bitrate,
											 double fps, int pixel_format)
{
	AVCodecContext *c;
	AVStream *st;
	int frame_rate, frame_rate_base;
	AVCodec *codec;


	st = av_new_stream(oc, 0);
	if (!st) {
		CV_WARN("Could not allocate stream");
		return NULL;
	}

#if LIBAVFORMAT_BUILD > 4628
	c = st->codec;
#else
	c = &(st->codec);
#endif

#if LIBAVFORMAT_BUILD > 4621
	c->codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_VIDEO);
#else
	c->codec_id = oc->oformat->video_codec;
#endif

	if(codec_id != CODEC_ID_NONE){
		c->codec_id = codec_id;
	}

    //if(codec_tag) c->codec_tag=codec_tag;
	codec = avcodec_find_encoder(c->codec_id);

	c->codec_type = AVMEDIA_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = bitrate;

	/* resolution must be a multiple of two */
	c->width = w;
	c->height = h;

	/* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
	frame_rate=(int)(fps+0.5);
	frame_rate_base=1;
	while (fabs((double)frame_rate/frame_rate_base) - fps > 0.001){
		frame_rate_base*=10;
		frame_rate=(int)(fps*frame_rate_base + 0.5);
	}
#if LIBAVFORMAT_BUILD > 4752
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
	/* adjust time base for supported framerates */
	if(codec && codec->supported_framerates){
		const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
		const AVRational *best=NULL;
		AVRational best_error= {INT_MAX, 1};
		for(; p->den!=0; p++){
			AVRational error= av_sub_q(req, *p);
			if(error.num <0) error.num *= -1;
			if(av_cmp_q(error, best_error) < 0){
				best_error= error;
				best= p;
			}
		}
		c->time_base.den= best->num;
		c->time_base.num= best->den;
	}
#else
	c->frame_rate = frame_rate;
	c->frame_rate_base = frame_rate_base;
#endif

	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = (PixelFormat) pixel_format;

	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO || c->codec_id == CODEC_ID_MSMPEG4V3){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
		/* avoid FFMPEG warning 'clipping 1 dct coefficients...' */
        c->mb_decision=2;
    }
#if LIBAVCODEC_VERSION_INT>0x000409
    // some formats want stream headers to be seperate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
    {
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
#endif

    return st;
}

int icv_av_write_frame_FFMPEG( AVFormatContext * oc, AVStream * video_st, uint8_t * outbuf, uint32_t outbuf_size, AVFrame * picture )
{

#if LIBAVFORMAT_BUILD > 4628
	AVCodecContext * c = video_st->codec;
#else
	AVCodecContext * c = &(video_st->codec);
#endif
	int out_size;
	int ret=-1;

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {

        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

		#ifndef PKT_FLAG_KEY
			#define PKT_FLAG_KEY AV_PKT_FLAG_KEY
		#endif
		
        pkt.flags |= PKT_FLAG_KEY; 
        pkt.stream_index= video_st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        //ret = av_write_frame(oc, &pkt);
		ret = pkt.size;
    } else {

        /* encode the image */
        out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);
#if LIBAVFORMAT_BUILD > 4752
            pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);
#else
			pkt.pts = c->coded_frame->pts;
#endif

            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= video_st->index;
            pkt.data= outbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            //ret = av_write_frame(oc, &pkt);
			ret = out_size;
        } else {
            ret = 0;
        }
    }
	return ret;
}

/// write a frame with FFMPEG
bool CvVideoWriter_FFMPEG::writeFrame( const unsigned char* data, int step, int width, int height, int cn, int origin )
{
	bool ret = false;

	// typecast from opaque data type to implemented struct
#if LIBAVFORMAT_BUILD > 4628
    AVCodecContext *c = video_st->codec;
#else
	AVCodecContext *c = &(video_st->codec);
#endif

#if LIBAVFORMAT_BUILD < 5231
    // It is not needed in the latest versions of the ffmpeg
    if( c->codec_id == CODEC_ID_RAWVIDEO && origin != 1 )
    {
        if( !temp_image.data )
        {
            temp_image.step = (width*cn + 3) & -4;
            temp_image.width = width;
            temp_image.height = height;
            temp_image.cn = cn;
            temp_image.data = malloc(temp_image.step*temp_image.height);
        }
        for( int y = 0; y < height; y++ )
            memcpy(temp_image.data + y*temp_image.step, data + (height-1-y)*step, width*cn);
        data = temp_image.data;
        step = temp_image.step;
    }
#endif

    // check parameters
    if (input_pix_fmt == PIX_FMT_BGR24) {
        if (cn != 3) {
            return false;
        }
    }
	else if (input_pix_fmt == PIX_FMT_GRAY8) {
        if (cn != 1) {
            return false;
        }
    }
	else {
        assert(false);
    }

	// check if buffer sizes match, i.e. image has expected format (size, channels, bitdepth, alignment)
/*#if LIBAVCODEC_VERSION_INT >= ((52<<16)+(37<<8)+0)
	assert (image->imageSize == avpicture_get_size( (PixelFormat)input_pix_fmt, image->width, image->height ));
#else
	assert (image->imageSize == avpicture_get_size( input_pix_fmt, image->width, image->height ));
#endif*/

	if ( c->pix_fmt != input_pix_fmt ) {
		assert( input_picture );
		// let input_picture point to the raw data buffer of 'image'
		avpicture_fill((AVPicture *)input_picture, (uint8_t *) data,
				(PixelFormat)input_pix_fmt, width, height);

#if !defined(HAVE_FFMPEG_SWSCALE)
		// convert to the color format needed by the codec
		if( img_convert((AVPicture *)picture, c->pix_fmt,
					(AVPicture *)input_picture, (PixelFormat)input_pix_fmt,
					width, height) < 0){
			return false;
		}
#else
		img_convert_ctx = sws_getContext(width,
		             height,
		             PIX_FMT_BGR24,
		             c->width,
		             c->height,
		             c->pix_fmt,
		             SWS_BICUBIC,
		             NULL, NULL, NULL);

		    if ( sws_scale(img_convert_ctx, input_picture->data,
		             input_picture->linesize, 0,
		             height,
		             picture->data, picture->linesize) < 0 )
		    {
               return false;
		    }
		sws_freeContext(img_convert_ctx);
#endif
	}
	else{
		avpicture_fill((AVPicture *)picture, (uint8_t *) data,
				(PixelFormat)input_pix_fmt, width, height);
	}

	out_size = icv_av_write_frame_FFMPEG( oc, video_st, outbuf, outbuf_size, picture);
	if (out_size>0) ret = true;

	return ret;
}

/// close video output stream and free associated memory
void CvVideoWriter_FFMPEG::close()
{
	unsigned i;

	// nothing to do if already released
	if ( !picture )
		return;

	/* no more frame to compress. The codec has a latency of a few
	   frames if using B frames, so we get the last frames by
	   passing the same picture again */
	// TODO -- do we need to account for latency here?

	/* write the trailer, if any */
	av_write_trailer(oc);

	// free pictures
#if LIBAVFORMAT_BUILD > 4628
	if( video_st->codec->pix_fmt != input_pix_fmt){
#else
	if( video_st->codec.pix_fmt != input_pix_fmt){
#endif
		if(picture->data[0])
           free(picture->data[0]);
        picture->data[0] = 0;
	}
	av_free(picture);

    if (input_picture) {
        av_free(input_picture);
    }

	/* close codec */
#if LIBAVFORMAT_BUILD > 4628
	avcodec_close(video_st->codec);
#else
	avcodec_close(&(video_st->codec));
#endif

	av_free(outbuf);

	/* free the streams */
	for(i = 0; i < oc->nb_streams; i++) {
		av_freep(&oc->streams[i]->codec);
		av_freep(&oc->streams[i]);
	}

	if (!(fmt->flags & AVFMT_NOFILE)) {
		/* close the output file */


#if LIBAVCODEC_VERSION_INT >= ((51<<16)+(49<<8)+0)
		url_fclose(oc->pb);
#else
		url_fclose(&oc->pb);
#endif

	}

	/* free the stream */
	av_free(oc);

    if( temp_image.data )
    {
        free(temp_image.data);
        temp_image.data = 0;
    }

	init();
}

/// Create a video writer object that uses FFMPEG
bool CvVideoWriter_FFMPEG::open( const char * filename, int fourcc,
		double fps, int width, int height, bool is_color )
{
	CodecID codec_id = CODEC_ID_NONE;
	int err, codec_pix_fmt, bitrate_scale=64;

    close();

	// check arguments
	assert (filename);
	assert (fps > 0);
	assert (width > 0  &&  height > 0);

	// tell FFMPEG to register codecs
	av_register_all ();

	/* auto detect the output format from the name and fourcc code. */

#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(53, 4, 0)
	fmt = av_guess_format(NULL, filename, NULL);
#else
	fmt = guess_format(NULL, filename, NULL);
#endif
	
	if (!fmt)
        return false;

	/* determine optimal pixel format */
    if (is_color) {
        input_pix_fmt = PIX_FMT_BGR24;
    }
	else {
        input_pix_fmt = PIX_FMT_GRAY8;
    }

	/* Lookup codec_id for given fourcc */
#if LIBAVCODEC_VERSION_INT<((51<<16)+(49<<8)+0)
    if( (codec_id = codec_get_bmp_id( fourcc )) == CODEC_ID_NONE )
        return false;
#else
	const struct AVCodecTag * tags[] = { codec_bmp_tags, NULL};
    if( (codec_id = av_codec_get_id(tags, fourcc)) == CODEC_ID_NONE )
        return false;
#endif

    // alloc memory for context
#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(53, 4, 0)
	oc = avformat_alloc_context();
#else
	oc = av_alloc_format_context();
#endif
	assert (oc);

	/* set file name */
	oc->oformat = fmt;
	snprintf(oc->filename, sizeof(oc->filename), "%s", filename);

	/* set some options */
	oc->max_delay = (int)(0.7*AV_TIME_BASE);  /* This reduces buffer underrun warnings with MPEG */

    // set a few optimal pixel formats for lossless codecs of interest..
    switch (codec_id) {
#if LIBAVCODEC_VERSION_INT>((50<<16)+(1<<8)+0)
    case CODEC_ID_JPEGLS:
        // BGR24 or GRAY8 depending on is_color...
        codec_pix_fmt = input_pix_fmt;
        break;
#endif
    case CODEC_ID_HUFFYUV:
        codec_pix_fmt = PIX_FMT_YUV422P;
        break;
    case CODEC_ID_MJPEG:
    case CODEC_ID_LJPEG:
      codec_pix_fmt = PIX_FMT_YUVJ420P;
      bitrate_scale = 128;
      break;
    case CODEC_ID_RAWVIDEO:
      codec_pix_fmt = input_pix_fmt;
      break;
    default:
        // good for lossy formats, MPEG, etc.
        codec_pix_fmt = PIX_FMT_YUV420P;
        break;
    }

	// TODO -- safe to ignore output audio stream?
	video_st = icv_add_video_stream_FFMPEG(oc, codec_id,
			width, height, width*height*bitrate_scale,
            fps, codec_pix_fmt);


	/* set the output parameters (must be done even if no
       parameters). */
    if (av_set_parameters(oc, NULL) < 0) {
        return false;
    }

    //dump_format(oc, 0, filename, 1);

    /* now that all the parameters are set, we can open the audio and
       video codecs and allocate the necessary encode buffers */
    if (!video_st){
        return false;
	}

    AVCodec *codec;
    AVCodecContext *c;

#if LIBAVFORMAT_BUILD > 4628
    c = (video_st->codec);
#else
    c = &(video_st->codec);
#endif

    c->codec_tag = fourcc;
    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        return false;
    }

	c->bit_rate_tolerance = c->bit_rate;

    /* open the codec */
    if ( (err=avcodec_open(c, codec)) < 0) {
		char errtext[256];
		sprintf(errtext, "Could not open codec '%s': %s", codec->name, icvFFMPEGErrStr(err));
		return false;
    }

    outbuf = NULL;

    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
		/* assume we will never get codec output with more than 4 bytes per pixel... */
		outbuf_size = width*height*4;
        outbuf = (uint8_t *) av_malloc(outbuf_size);
    }

	bool need_color_convert;
	need_color_convert = (c->pix_fmt != input_pix_fmt);

    /* allocate the encoded raw picture */
    picture = icv_alloc_picture_FFMPEG(c->pix_fmt, c->width, c->height, need_color_convert);
    if (!picture) {
        return false;
    }

    /* if the output format is not our input format, then a temporary
       picture of the input format is needed too. It is then converted
	   to the required output format */
	input_picture = NULL;
    if ( need_color_convert ) {
        input_picture = icv_alloc_picture_FFMPEG(input_pix_fmt, c->width, c->height, false);
        if (!input_picture) {
            return false;
        }
    }

	/* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&oc->pb, filename, URL_WRONLY) < 0) {
            return false;
        }
    }

    /* write the stream header, if any */
    av_write_header( oc );

	return true;
}






CvVideoWriter_FFMPEG* cvCreateVideoWriter_FFMPEG( const char* filename, int fourcc, double fps,
                                                  int width, int height, int isColor )
{
    CvVideoWriter_FFMPEG* writer = (CvVideoWriter_FFMPEG*)malloc(sizeof(*writer));
    writer->init();
    if( writer->open( filename, fourcc, fps, width, height, isColor != 0 ))
        return writer;
    writer->close();
    free(writer);
    return 0;
}


void cvReleaseVideoWriter_FFMPEG( CvVideoWriter_FFMPEG** writer )
{
    if( writer && *writer )
    {
        (*writer)->close();
        free(*writer);
        *writer = 0;
    }
}


int cvWriteFrame_FFMPEG( CvVideoWriter_FFMPEG* writer,
                         const unsigned char* data, int step,
                         int width, int height, int cn, int origin)
{
    return writer->writeFrame(data, step, width, height, cn, origin);
}


#endif