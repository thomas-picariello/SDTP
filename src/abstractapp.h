#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>



class AbstractApp : public QWidget
{

    Q_OBJECT

public:
    AbstractApp(QWidget *parent = 0);
    ~AbstractApp();

    int getAppID();

public slots :
    void dataToRead(QByteArray data);


signals :
    void dataToSend(QByteArray);

protected :

    int mAppID;

};

#endif // ABSTRACTAPP_H
