#ifndef FFMPEGS_H
#define FFMPEGS_H

#include <QObject>
extern "C" {
    #include <libavformat/avformat.h>
}
struct RawVideoFile
{
    char * filename;
    int w;
    int h;
    AVPixelFormat fmt;
};

struct RawVideoFrame
{
    void * pixels;
    int w;
    int h;
    AVPixelFormat fmt;
};


class FFmpegs : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegs(QObject *parent = nullptr);
    static void transformFile(RawVideoFile &in,RawVideoFile &out);

    static void transformFile(RawVideoFrame &in,RawVideoFrame &out);
signals:

};

#endif // FFMPEGS_H
