#ifndef PACKETAGENT_H
#define PACKETAGENT_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QPair>







class PacketAgent: public QObject
{

    Q_OBJECT

public:
    enum APPID{Root,Chat,Voip,System,Unknown}; // 0-10 are reserved IDs.

    PacketAgent(QPair<QByteArray,QByteArray> key);// key = key + IV
    PacketAgent();
    ~PacketAgent();

public slots :
    void incomingdata(QByteArray data);//link
    void writedata(QByteArray data);//app

signals:
    void senddata(QByteArray data);//link
    void readdata(QByteArray data);//app


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
