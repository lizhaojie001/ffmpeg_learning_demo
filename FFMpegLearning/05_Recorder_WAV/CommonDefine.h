#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H
#include <stdint.h>

#define HOME

#ifdef Q_OS_WIN    // PCM文件的文件名
#define FILENAME "E:/out.pcm"
#define OUTWAVFILENAME "E:/out2.wav"

// 格式名称
#define FMT_NAME "dshow"
// 设备名称
#define DEVICE_NAME "audio=麦克风 (USBAudio2.0)"
#else

#define FMT_NAME "avfoundation"

#ifdef HOME
#define FILENAME "/Users/macbookpro/Downloads/out.pcm"
#define OUTWAVFILENAME "/Users/macbookpro/Downloads/out2.wav"
#define DEVICE_NAME ":0"
#else
#define FILENAME "/Users/xdf_yanqing/Downloads/out.pcm"
#define OUTWAVFILENAME "/Users/xdf_yanqing/Downloads/out2.wav"

#define DEVICE_NAME ":4"
#endif
#endif


#define AUDIO_PCM_FORMAT 1
struct WAVHeader {
       uint8_t ChunkID[4] = { 'R','I','F','F'};
       uint32_t ChunkSize;
       uint8_t Format[4] = {'W','A','V','E'};

       uint8_t FmtchunkID[4] = {'f','m','t',' '};
       uint32_t FmtchunkSize = 16;
//       音频数据格式
       uint16_t AudioFormat = AUDIO_PCM_FORMAT; //pcm
       uint16_t NumChannels;

       uint32_t SampleRate;
       //比特率 = SampleRate * BlockAlign;
       uint32_t ByteRate;
       //单个样本多少字节  = BitsPerSample * NumChannels >> 3
       uint16_t BlockAlign;
       //位深
       uint16_t BitsPerSample;

       uint8_t DatachunkID[4] = {'d','a','t','a'};
       uint32_t DatachunkSize;
};



#endif // COMMONDEFINE_H
