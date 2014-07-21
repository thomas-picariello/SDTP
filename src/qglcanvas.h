#ifndef QGLCANVAS_H
#define QGLCANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QDebug>



class QGLCanvas : public QGLWidget
{
public:
    QGLCanvas(QWidget* parent = NULL);
public slots:
    void setImage(QImage image);
protected:
    void paintEvent(QPaintEvent*);
private:
    QImage img;
};

#endif // QGLCANVAS_H
