
/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.


*
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c

*/
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <switch.h>

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;//, *audio_dec_ctx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static AVStream *video_stream = NULL;//, *audio_stream = NULL;
static uint8_t *video_dst_data[4] = {NULL};
static int video_dst_linesize[4];
static int video_dst_bufsize;
static int video_stream_idx = -1;//, audio_stream_idx = -1;
static AVFrame *frame = NULL;
static AVFrame *rgbframe = NULL;
static struct SwsContext *ctx_sws = NULL;
static AVPacket pkt;
static int video_frame_count = 0;
//static int audio_frame_count = 0;

/*Enable or disable frame reference counting. You are not supposed to support
* both paths in your application but pick the one most appropriate to your
* needs. Look for the use of refcount in this example to see what are the
* differences of API usage between them.
*/
static int refcount = 0;

static int decode_packet(int *got_frame, int cached)
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
                /*
                 To handle this change, one could call av_image_alloc again and
                                 * decode the following frames into another rawvideo file.
                */

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
            //printf("video_frame%s n:%d coded_n:%d\n",
            //       cached ? "(cached)" : "",
            //       video_frame_count++, frame->coded_picture_number);
            if (++video_frame_count % 60 == 0)
            {
                printf("%d\n", video_frame_count);
            }

            if (ctx_sws == NULL)
                ctx_sws = sws_getContext(frame->width, frame->height, pix_fmt, frame->width, frame->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);

            u8 *fbuf = gfxGetFramebuffer(NULL, NULL);

            sws_scale(ctx_sws, (const uint8_t *) frame->data, frame->linesize, 0, frame->height, &fbuf, rgbframe->linesize);

            //memcpy(fbuf, rgbframe->data[0], 1280 * 720 * 4);
            gfxFlushBuffers();
            gfxSwapBuffers();
            /*
             copy decoded frame to destination buffer:
                         * this is required since rawvideo expects non aligned data
            */

            // --> maybe do other stuff with the frames instead?
            //av_image_copy(video_dst_data, video_dst_linesize,
            //              (const uint8_t **) (frame->data), frame->linesize,
            //              pix_fmt, width, height);

            //write to rawvideo file

            //fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
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
    AVDictionary *opts = NULL;
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
        //Init the decoders, with or without reference counting

        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0)
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
    int got_frame;

#define URL "tcp://0.0.0.0:2222"

    // setting TCP input options
    AVDictionary *opts = 0;
    av_dict_set(&opts, "listen", "1", 0); // set option for listening
    //#define TCP_RECV_BUFFER "500000"
    //av_dict_set(&opts, "recv_buffer_size", TCP_RECV_BUFFER, 0);       // set option for size of receive buffer

    //open input file, and allocate format context

    ret = avformat_open_input(&fmt_ctx, URL, 0, &opts);
    if (ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);

        fprintf(stderr, "Input Error %s\n", errbuf);
        while (1)
            ;
    }

    //retrieve stream information

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        while (1)
            ;
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
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_packet_unref(&orig_pkt);
    }

    //flush cached frames

    pkt.data = NULL;
    pkt.size = 0;
    do
    {
        decode_packet(&got_frame, 1);
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


int main(int argc, char **argv)
{
    pcvInitialize();
    pcvSetClockRate(PcvModule_Cpu, 1785000000);

    static const SocketInitConfig socketInitConf = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = 0x8000,
        .tcp_rx_buf_size = 0x10000,
        .tcp_tx_buf_max_size = 0x40000,
        .tcp_rx_buf_max_size = 0x40000,

        .udp_tx_buf_size = 0xA2400,
        .udp_rx_buf_size = 0xAA500,

        .sb_efficiency = 4,

        .serialized_out_addrinfos_max_size = 0x1000,
        .serialized_out_hostent_max_size = 0x200,
        .bypass_nsd = false,
        .dns_timeout = 0,
    };
    socketInitialize(&socketInitConf);

    nxlinkStdio();
    gfxInitDefault();

    avformat_network_init();

    while(appletMainLoop()) {
        handleVid();
    }

    avformat_network_deinit();

    gfxExit();
    pcvExit();
    socketExit();
}
