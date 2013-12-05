#ifndef MESSENGER_H
#define MESSENGER_H

#include <QDebug>
#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>



class Messenger: public QObject
{
    Q_OBJECT
public:
    explicit Messenger(QObject *parent = 0);
    ~Messenger();

signals:
    void sendMessage(QByteArray data);

public slots:
    void displayMessage(QString);
    void onSend();

private :
    QWidget *messenger;
    QLabel *messageDisplay;
    QLineEdit *getmsg;
    QPushButton *sendmsg;
    QStringList *msglist;
    QHBoxLayout *sendlayout;
    QVBoxLayout *msglayout;


};

#endif // MESSENGER_H
