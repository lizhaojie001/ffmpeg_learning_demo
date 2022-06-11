#include "customslider.h"
#include <QStyle>
#include <QMouseEvent>


CustomSlider::CustomSlider(QWidget *parent):QSlider(parent)
{

}

void CustomSlider::mousePressEvent(QMouseEvent *e)
{

    int value = minimum() + QStyle::sliderValueFromPosition(minimum(),maximum(),e->pos().x(),width());
    setValue(value);
    QSlider::mousePressEvent(e);
}
