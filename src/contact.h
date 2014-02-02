#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QHostAddress>
//#include <QSharedData>

class Contact//: public QSharedData
{
public: 
    Contact(int id = getNextAvailableID(),
            QString name = QString(),
            QString hostName = QString(),
            QHostAddress ip = QHostAddress(),
            quint16 port = 0,
            QByteArray key = QByteArray());

    static Contact* findById(int id);
    static QList<Contact*> findByName(QString name);
    static QList<Contact*> findByIp(QHostAddress ip);
    static QList<Contact*> findByHostName(QString hostName);
    static Contact* findByKey(QByteArray key);
    static QList<Contact*> getContactList();

    static int getNextAvailableID();

    int getId() const;
    QString getName() const;
    QHostAddress getIpAddress() const;
    QString getHostName() const;
    quint16 getPort() const;
    QByteArray getKey() const;

    void setId(int id);
    void setName(QString name);
    void setIpAddress(QHostAddress ip);
    void setHostName(QString hostName);
    void setPort(quint16 port);
    void setKey(QByteArray key);
    void save();
    void erase();

private :
    int mId;
    quint16 mPort;
    QString mName, mHostName;
    QHostAddress mIp;
    QByteArray mKey;
};

#endif // CONTACT_H
