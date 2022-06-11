#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H


#define ERROR(ret) \
    char errbuf[4096] = {0};\
    av_strerror(ret, errbuf, sizeof (errbuf));

#define HOME

#ifdef Q_OS_WIN    // PCM文件的文件名
#define FILENAME "E:/out.pcm"
#define OUTWAVFILENAME "E:/out2.wav"
#define OUTPCMFILENAME "E:/out_48000_f32le_1.pcm"
// 格式名称
#define FMT_NAME "dshow"
// 设备名称
#define DEVICE_NAME "audio=麦克风 (USBAudio2.0)"
#else

#define FMT_NAME "avfoundation"

#ifdef HOME
#define FILENAME "/Users/macbookpro/Downloads/out.pcm"
#define OUTWAVFILENAME "/Users/macbookpro/Downloads/out2.wav"
#define OUTPCMFILENAME "/Users/macbookpro/Downloads/out_44100_s16le_1.pcm"

#define DEVICE_NAME ":0"
#else
#define FILENAME "/Users/xdf_yanqing/Downloads/out.pcm"
#define OUTWAVFILENAME "/Users/xdf_yanqing/Downloads/out2.wav"

#define DEVICE_NAME ":4"
#endif
#endif




#endif // COMMONDEFINE_H
