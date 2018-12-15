#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

#include <switch.h>

#include "video.h"

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL; //, *audio_dec_ctx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static AVStream *video_stream = NULL; //, *audio_stream = NULL;
static uint8_t *video_dst_data[4] = {NULL};
static int video_dst_linesize[4];
static int video_dst_bufsize;
static int video_stream_idx = -1; //, audio_stream_idx = -1;
static AVFrame *rgbframe = NULL;
static AVPacket pkt;
static int video_frame_count = 0;
//static int audio_frame_count = 0;

#include "network.h"
#include "renderer.h"

static int decode_packet(AVFrame* frame, int *got_frame, int cached)
{
    int ret = 0;
    *got_frame = 0;
    if (pkt.stream_index == video_stream_idx)
    {
        //decode video frame

        // deprecated --> use avcodec_send_packet() and avcodec_receive_frame() instead?
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0)
        {
            fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
            return ret;
        }
        if (*got_frame)
        {
            if (frame->width != width || frame->height != height ||
                frame->format != pix_fmt)
            {
                fprintf(stderr, "Error: Width, height and pixel format have to be "
                                "constant in a rawvideo file, but the width, height or "
                                "pixel format of the input video changed:\n"
                                "old: width = %d, height = %d, format = %s\n"
                                "new: width = %d, height = %d, format = %s\n",
                        width, height, av_get_pix_fmt_name(pix_fmt),
                        frame->width, frame->height,
                        av_get_pix_fmt_name(frame->format));
                return -1;
            }

            if (++video_frame_count % 60 == 0)
            {
                printf("%d\n", video_frame_count);
            }
            drawFrame(frame, rgbframe, pix_fmt);
        }
    }
    return ret;
}

static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not find %s stream in input file \n",
                av_get_media_type_string(type));
        return ret;
    }
    else
    {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];
        // find decoder for the stream

        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (dec == NULL)
        {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        // Allocate a codec context for the decoder

        *dec_ctx = avcodec_alloc_context3(dec);
        if (*dec_ctx == NULL)
        {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        /*
        Copy codec parameters from input stream to output codec context
        */

        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
        {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }
        //Init the decoders, without reference counting

        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
        {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }
    return 0;
}

int handleVid()
{
    int ret = 0;
    int got_frame = 0;
    AVFrame* frame;

    // setting TCP input options
    AVDictionary *opts = 0;
    av_dict_set(&opts, "listen", "1", 0); // set option for listening
    av_dict_set(&opts, "probesize", "10000", 0);
    //av_dict_set(&opts, "recv_buffer_size", TCP_RECV_BUFFER, 0);       // set option for size of receive buffer

    //open input file, and allocate format context
    ret = avformat_open_input(&fmt_ctx, URL, 0, &opts);
    if (ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);

        fprintf(stderr, "Input Error %s\n", errbuf);
        goto end;
    }

    //retrieve stream information

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        goto end;
    }

    // Context for the video
    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        video_stream = fmt_ctx->streams[video_stream_idx];

        //allocate image where the decoded image will be put

        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;

        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             width, height, pix_fmt, 1);
        if (ret < 0)
        {
            char errbuf[100];
            av_strerror(ret, errbuf, 100);
            fprintf(stderr, "Could not allocate raw video buffer %s\n", errbuf);
            goto end;
        }
        video_dst_bufsize = ret;
    }

    //dump input information to stderr
    //av_dump_format(fmt_ctx, 0, URL, 0);

    if (video_stream == NULL)
    {
        fprintf(stderr, "Could not find stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    rgbframe = av_frame_alloc();
    rgbframe->width = 1280;
    rgbframe->height = 720;
    rgbframe->format = AV_PIX_FMT_RGBA;
    av_image_alloc(rgbframe->data, rgbframe->linesize, rgbframe->width, rgbframe->height, rgbframe->format, 32);

    frame = av_frame_alloc();

    if (frame == NULL)
    {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    //initialize packet, set data to NULL, let the demuxer fill it

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    //read frames from the file
    while (av_read_frame(fmt_ctx, &pkt) >= 0)
    {
        AVPacket orig_pkt = pkt;
        do
        {
            ret = decode_packet(frame, &got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } 
        while (pkt.size > 0);
        av_packet_unref(&orig_pkt);
    }

    //flush cached frames
    pkt.data = NULL;
    pkt.size = 0;
    do
    {
        decode_packet(frame, &got_frame, 1);
    } while (got_frame);

    printf("Demuxing succeeded.\n");

end:
    avcodec_free_context(&video_dec_ctx);
    //avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);

    av_frame_free(&frame);
    av_free(video_dst_data[0]);
    return ret;
}