#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H



#ifdef Q_OS_WIN    // PCM文件的文件名
#define FILENAME "E:/out.pcm"
#else
#define FILENAME "/Users/xdf_yanqing/Downloads/out.pcm"
#endif




#ifdef Q_OS_WIN
    // 格式名称
    #define FMT_NAME "dshow"
    // 设备名称
    #define DEVICE_NAME "audio=麦克风 (USBAudio2.0)"
#else
    #define FMT_NAME "avfoundation"
    #define DEVICE_NAME ":4"
#endif



#endif // COMMONDEFINE_H
