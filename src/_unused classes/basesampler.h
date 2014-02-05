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
    
signals:
    void sampleAvailable(QByteArray *returnSample);
    
public slots:
    virtual void getSample() = 0;

protected:
    QByteArray *mSample;

private:

};

#endif // BASESAMPLER_H
