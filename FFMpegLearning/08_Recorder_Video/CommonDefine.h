#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H
#include <stdint.h>

#define HOME

#ifdef Q_OS_WIN    // PCM文件的文件名
#define FILENAME "E:/out.yuv"

// 格式名称
#define FMT_NAME "gdigrab"
// 设备名称
#define DEVICE_NAME "desktop"
#else

#define FMT_NAME "avfoundation"

#ifdef HOME
#define FILENAME "/Users/macbookpro/Downloads/out.yuv"
#define DEVICE_NAME "0"
#else
#define FILENAME "/Users/xdf_yanqing/Downloads/out.pcm"
#define OUTWAVFILENAME "/Users/xdf_yanqing/Downloads/out2.wav"

#define DEVICE_NAME "0"
#endif
#endif


#define AUDIO_PCM_FORMAT 1


#endif // COMMONDEFINE_H
