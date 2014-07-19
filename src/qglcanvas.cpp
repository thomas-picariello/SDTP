#include "qglcanvas.h"

QGLCanvas::QGLCanvas(QWidget* parent)
    : QGLWidget(parent)
{
}

void QGLCanvas::setImage(QImage *image)
{
    img = image->copy(QRect(0,0,image->width(),image->height()));
    update();

}

void QGLCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if(!img.isNull()){


        //Set the painter to use a smooth scaling algorithm.
        p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

        p.drawImage(this->rect(), img);

    }
}
