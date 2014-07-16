#include "flattouchstyle.h"

int FlatTouchStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (metric == QStyle::PM_SmallIconSize) //display bigger menu icons
        return 24;
    else
        return QProxyStyle::pixelMetric(metric, option, widget);
}
