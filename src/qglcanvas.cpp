#include "qglcanvas.h"

QGLCanvas::QGLCanvas(QWidget* parent)
    : QGLWidget(parent)
{

}

void QGLCanvas::setImage(QImage image)
{

    img = image;
    update();

}

void QGLCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if(!img.isNull()){
        //Set the painter to use a smooth scaling algorithm.
        p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

        if(this->rect().width()/2 < img.width()){
            img.scaledToWidth(this->rect().width()/2);
        }
        if(this->rect().height() < img.height()){
            img.scaledToHeight(this->rect().height());
        }

        p.drawImage(QRect(0,0,img.width()/2,img.height()/2) , img);
    }
}
