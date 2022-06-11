#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>

class CustomSlider : public QSlider
{
public:
    CustomSlider(QWidget * parent = nullptr);
private:
    void mousePressEvent(QMouseEvent * e) override;
};

#endif // CUSTOMSLIDER_H
