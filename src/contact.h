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
    Contact(int id = getNextAvailableID(),
            QString name = QString(),
            QString host = QString(),
            quint16 port = 0,
            QByteArray key = QByteArray());

    static int getNextAvailableID();

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
