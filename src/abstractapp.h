#ifndef ABSTRACTAPP_H
#define ABSTRACTAPP_H

#include <QWidget>



class AbstractApp : public QWidget
{

    Q_OBJECT

public:
    AbstractApp(QWidget *parent = 0);
    ~AbstractApp();

public slots :
    void dataToRead(QByteArray data);


signals :
    void dataToSend(QByteArray);

private :



};

#endif // ABSTRACTAPP_H
