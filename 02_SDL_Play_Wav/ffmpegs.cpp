#include "ffmpegs.h"
#include <QFile>
#include <QDebug>
FFmpegs::FFmpegs(QObject *parent)
    : QObject{parent}
{

}

void FFmpegs::pcm2wav(WAVHeader &header, const char *pcmFilename, const char *wavFilename)
{
//    BitsPerSample * NumChannels >> 3
    header.BlockAlign =  header.NumChannels * header.BitsPerSample >> 3;
    header.ByteRate = header.SampleRate * header.BlockAlign >> 3;
    //打开文件pcm
    QFile pcmFile(pcmFilename);
    if (!pcmFile.open(QFile::ReadOnly)) {
        qDebug() << "文件打开失败" << pcmFilename;
        return;
    }
    header.DatachunkSize = pcmFile.size();
    header.ChunkSize = header.DatachunkSize
            + sizeof (WAVHeader) - 8;
    //打开wav
    QFile wavFile(wavFilename);
    if (!wavFile.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << wavFilename;
        pcmFile.close();
        return;
    }

    //写入头文件
    wavFile.write((const char *)&header, sizeof(WAVHeader));
    //写入pcm数据
    char buffer[1024];
    uint64_t len;
    while ((len = pcmFile.read(buffer,sizeof(buffer))) > 0) {
             wavFile.write(buffer,len);
    }


    pcmFile.close();
    wavFile.close();


}
