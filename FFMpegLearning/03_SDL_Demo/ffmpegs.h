#ifndef FFMPEGS_H
#define FFMPEGS_H
#include <QObject>

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


class FFmpegs : public QObject
{
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void pcm2wav(WAVHeader &header , const char * pcmFilename , const char * wavFilename);
signals:

};

#endif // FFMPEGS_H
