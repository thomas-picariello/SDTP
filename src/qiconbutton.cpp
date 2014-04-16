#include "qiconbutton.h"

QIconButton::QIconButton(QWidget *parent) :
    QPushButton(parent)
{
    setMouseTracking(true);
    //set density aware iconsize;
    float iconScaleFactor = 96/logicalDpiX();
    setIconSize(QSize(iconSize().height()* iconScaleFactor,
                      iconSize().width() * iconScaleFactor));
    show();
}

void QIconButton::enterEvent(QEvent *event){
    mButtonState = Hover;
}

void QIconButton::leaveEvent(QEvent *event){
    mButtonState = Normal;
    clearFocus();
}

void QIconButton::mousePressEvent(QMouseEvent *event){
    mButtonState = Pressed;
}

void QIconButton::mouseReleaseEvent(QMouseEvent *event){
    mButtonState = Hover;
    clearFocus();
}

void QIconButton::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPixmap iconPixmap;
    switch(mButtonState){
    case Disabled:
        iconPixmap = icon().pixmap(iconSize(), QIcon::Disabled);
        break;
    case Normal:
        iconPixmap = icon().pixmap(iconSize(), QIcon::Normal);
        break;
    case Hover:
        iconPixmap = icon().pixmap(iconSize(), QIcon::Active);
        break;
    case Pressed:
        iconPixmap = icon().pixmap(iconSize(), QIcon::Selected);
        break;
    }
    painter.drawPixmap(0,0, iconPixmap);
}
