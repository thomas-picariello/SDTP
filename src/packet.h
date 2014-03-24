#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QDebug>



class Packet
{
public:
    enum APPID{MESSENGER,VOIP ,SYSTEM,SYNC,UNKNOWN, ERROR}; // 0-10 are reserved IDs.

    Packet(QByteArray content,APPID appid);
    Packet(QByteArray data);

    const QByteArray content();
    const APPID appid();
    const QByteArray data();

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
    APPID mAppID;
    QByteArray mData;
    QByteArray mHash;

};

#endif // PACKET_H
