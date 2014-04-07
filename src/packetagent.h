#ifndef PACKETAGENT_H
#define PACKETAGENT_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QPair>



class PacketAgent
{
public:
    enum APPID{ROOT,MESSENGER,VOIP,SYNC,UNKNOWN, ERROR}; // 0-10 are reserved IDs.

    PacketAgent(QPair<QByteArray,QByteArray> key);// key = key + IV

    bool login( QObject* parent );
    bool logout( QObject* parent );

public slots :
    void send(QByteArray data, QObject* parent);
    QByteArray read(QObject* parent);

signals:
    QByteArray newdata(); // send signal specifically to one app...

private:

    void encrypt();
    void decrypt();
    void addheader();

    void addHash();
    void makeHash();

    void getHash();
    void parseHeader();
    bool verifyHash();

    QByteArray mContent;
    QByteArray mData;
    QByteArray mHash;
    QByteArray mKey;
    QByteArray mIV;

};

#endif // PACKET_H
