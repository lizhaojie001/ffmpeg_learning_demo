#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>
#include <QObject>
class CustomSlider : public QSlider
{
    Q_OBJECT
public:
    explicit CustomSlider(QWidget *parent = nullptr);

signals:
    void handclicked(CustomSlider *p );
private:
   virtual  void mousePressEvent(QMouseEvent * e) override;

};

#endif // CUSTOMSLIDER_H
