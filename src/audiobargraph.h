
/**
 * @brief AudioBargraph represents a GUI object used to display Audio signals.
 */

#ifndef AUDIOBARGRAPH_H
#define AUDIOBARGRAPH_H

#include <QProgressBar>
#include <QTimer>
#include <QtMath>

class AudioBargraph : public QProgressBar
{
    Q_OBJECT
public:
    static const int INTERPOLATION_INTERVAL = 33;

    explicit AudioBargraph(QWidget *parent = 0);

    int getInterpolationTime() const;
    int value() const;
    void setInterpolationTime(int time);
    void setValue(int value);

private slots:
    void interpolate();

private:
    int m_previousValue;
    int m_targetValue;
    int m_interpolationTime;
    float m_currentValue;
    QTimer m_interpolationTimer;

};

#endif // AUDIOBARGRAPH_H
