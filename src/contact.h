#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QHostAddress>
#include <QHostInfo>
//#include <QSharedData>

class Contact: public QObject
{
    Q_OBJECT
public: 
    Contact(int id = getNextAvailableID(),
            QString name = QString(),
            QString hostName = QString(),
            QHostAddress ip = QHostAddress(),
            quint16 port = 0,
            QByteArray key = QByteArray());

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

signals:
    void unableToResolve(QString error);

public slots:
    void onResolve(QHostInfo &hostInfo);

private :
    int mId;
    quint16 mPort;
    QString mName, mHostName;
    QHostAddress mIp;
    QByteArray mKey;

    Q_DISABLE_COPY(Contact)
};

#endif // CONTACT_H
