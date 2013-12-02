#ifndef BASESAMPLER_H
#define BASESAMPLER_H

#include <QObject>
#include <QByteArray>

class BaseSampler : public QObject
{
    Q_OBJECT
public:
    explicit BaseSampler(QObject *parent = 0);
    ~BaseSampler();
    void getSample();
    
signals:
    void sampleAvailable(QByteArray *returnSample);
    
public slots:

protected:
    QByteArray *mSample;

private:

};

#endif // BASESAMPLER_H
