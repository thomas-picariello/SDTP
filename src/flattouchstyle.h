/**
 * @brief FlatTouchStyle is used to manipulate size and position of various menu Items
 */

#ifndef FLATTOUCHSTYLE_H
#define FLATTOUCHSTYLE_H

#include <QProxyStyle>

class FlatTouchStyle : public QProxyStyle
{
public:
    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
};

#endif // FLATTOUCHSTYLE_H
