#ifndef QICONBUTTON_H
#define QICONBUTTON_H

#include <QDebug>
#include <QPushButton>
#include <QPainter>

class QIconButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonState{
        Disabled,
        Normal,
        Hover,
        Pressed
    };

    explicit QIconButton(QWidget *parent = 0);

signals:
    void hovered();
    void leaved();

public slots:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    ButtonState mButtonState;

};

#endif // QICONBUTTON_H
