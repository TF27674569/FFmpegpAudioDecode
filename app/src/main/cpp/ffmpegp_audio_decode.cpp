#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>


#define TAG "FFmpegp"

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,##__VA_ARGS__);

#define BUFFER_SIZE 88200 // 16bit 44100hz 8bit = 1byte


extern "C"{

//封装视频格式
#include "libavformat/avformat.h"
// 解码库
#include "libavcodec/avcodec.h"
// 缩放
#include "libswscale/swscale.h"

#include "libswresample/swresample.h"

}


extern "C"
JNIEXPORT void JNICALL
Java_com_sample_audiod_ffmpegp_AudioUtils_decode(JNIEnv *env, jclass type, jstring input_,
                                                 jstring output_) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    /**
     * 1 注册组建
 　　 * 2 打开输入音频文件
 　　 * 3 获取音频信息
 　　 * 4 获取音频解码器 (在此之前需要先获取到音频流信息)
 　　 * 5 打开音频解码器
 　　 * 6 一帧一帧读取压缩音频数据AVPacket 一帧一帧的音频包 （之前需要设置重采样参数）
 　　 *　　6.1 重采样 读取解码buffer
 　　 *　　6.2 写入文件
 　  * 7 关闭释放资源
     */

    av_register_all();

    AVFormatContext* avFormatContext = avformat_alloc_context();

    if (avformat_open_input(&avFormatContext,input,NULL,NULL) != 0){
        LOGE("无法打开音频文件")
        return;
    }

    if (avformat_find_stream_info(avFormatContext,NULL) < 0){
        LOGE("无法获取音频信息")
        return;
    }

    int audio_index = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
            goto check;
        }
    }

    check:

    if (audio_index == -1){
        LOGE("无法获取音频流")
        return;
    }

    AVCodecContext* avCodecContext = avFormatContext->streams[audio_index]->codec;
    AVCodec* avCodec = avcodec_find_decoder(avCodecContext->codec_id);

    if (avCodec == NULL){
        LOGE("无法获取解码器")
        return;
    }

    if(avcodec_open2(avCodecContext,avCodec,NULL) < 0){
        LOGE("无法打开解码器")
        return;
    }

    //一帧一帧读取压缩音频数据AVPacket
    AVPacket* avPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    AVFrame* avFrame = av_frame_alloc();

    int got_frame,frame_count = 0;

    // 在c里面结构体拿到内存空间后需要对这个结构体设置参数
    SwrContext* swrContext = swr_alloc();

    /**----------重采样参数设置-------------**/
    // 输入声道布局
    int64_t  in_ch_layout = avCodecContext->channel_layout;
    // 输入采样格式
    enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
    // 输入采样率
    int  in_sample_rate = avCodecContext->sample_rate;

    // 输出参数我们采用16bit 44100hz pcm双声道立体声 应为麦克风采集就是pcm
    // 输出声道布局 立体声
    int64_t  out_ch_layout = AV_CH_LAYOUT_STEREO;
    // 输出采样格式 16bt
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    // 输出采样率 44100hz
    int  out_sample_rate = BUFFER_SIZE >> 1;

    swr_alloc_set_opts(swrContext,
                       out_ch_layout,out_sample_fmt,out_sample_rate,
                       in_ch_layout,in_sample_fmt,in_sample_rate,
                        0,NULL// 没有数据偏移
                        );
    // 通过init函数使参数设置成功
    swr_init(swrContext);
    /**----------------------------------**/
    // 输出的声道个数
    int out_ch_layout_count = av_get_channel_layout_nb_channels(out_ch_layout);
    uint8_t* out_buffer = (uint8_t*)av_malloc(BUFFER_SIZE);

    FILE* file = fopen(output,"wb");
    while(av_read_frame(avFormatContext,avPacket) >=0){
        // 读取音频帧
        if(avcodec_decode_audio4(avCodecContext,avFrame,&got_frame,avPacket)<0){
            LOGE("解码完成")
        }

        if (got_frame>0){
            LOGE("解码:%d帧",frame_count++)

            //重采样
            swr_convert(swrContext,
                        &out_buffer, //输出数据保存的buffer
                        BUFFER_SIZE,// 上面buffer有多大 c 指针退化
                        (const uint8_t**)(avFrame->data),// 需要转换的数据
                        avFrame->nb_samples//采样的个数
                        );
            // buffer里面读了多少数据
            int out_buffer_size = av_samples_get_buffer_size(NULL,out_ch_layout_count,avFrame->nb_samples,out_sample_fmt,1);

            fwrite(out_buffer,1,out_buffer_size,file);
        }
        av_free_packet(avPacket);
    }



    fclose(file);
    av_frame_free(&avFrame);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}