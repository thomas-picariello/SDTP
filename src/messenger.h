#ifndef MESSENGER_H
#define MESSENGER_H

#include <QDebug>
#include <QByteArray>
#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"

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
    CryptoPP::AES::Encryption mEncryptor;
    CryptoPP::AES::Decryption mDecryptor;
};

#endif // MESSENGER_H
