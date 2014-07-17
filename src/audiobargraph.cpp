#include "audiobargraph.h"

AudioBargraph::AudioBargraph(QWidget *parent) :
    QProgressBar(parent),
    m_previousValue(0),
    m_targetValue(0),
    m_interpolationTime(0),
    m_currentValue(0)
{
    connect(&m_interpolationTimer, &QTimer::timeout, this, &AudioBargraph::interpolate);
    m_interpolationTimer.start(INTERPOLATION_INTERVAL);
}

int AudioBargraph::getInterpolationTime() const{
    return m_interpolationTime;
}

int AudioBargraph::value() const{
    return m_targetValue;
}

void AudioBargraph::setInterpolationTime(int time){
    m_interpolationTime = time;
}

void AudioBargraph::setValue(int value){
    m_previousValue = m_currentValue;
    m_targetValue = value;
}

void AudioBargraph::interpolate(){
    //linear interpolation
    if(m_currentValue != m_targetValue){
        float step = (m_targetValue-m_previousValue)/(m_interpolationTime/(float)INTERPOLATION_INTERVAL);
        m_currentValue += step;
        if(qAbs(m_currentValue - m_targetValue) < qAbs(step))
            m_currentValue = m_targetValue;
        QProgressBar::setValue(m_currentValue);
        update();
    }
}
