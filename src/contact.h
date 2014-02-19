#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QHostAddress>
#include <QHostInfo>

class Contact: public QObject
{
    Q_OBJECT
public: 
    explicit Contact(QObject *parent = 0);
    explicit Contact(int id,
            QString name,
            QString host,
            quint16 port,
            QByteArray key,
            QObject *parent = 0);

    int getNextAvailableID();

    int getId() const;
    QString getName() const;
    QString getHost() const;
    quint16 getPort() const;
    QByteArray getKey() const;
    bool isResolving() const;

    void setId(int id);
    void setName(QString name);
    void setHost(QString host);
    void setPort(quint16 port);
    void setKey(QByteArray key);
    void save();
    void erase();

private :
    int mId;
    quint16 mPort;
    QString mName, mHost;
    QByteArray mKey;

    Q_DISABLE_COPY(Contact)
};

#endif // CONTACT_H
