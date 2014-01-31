#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QSettings>
#include <QStringList>

class Contact
{
public:
    Contact(QString name = QString(),
            QString ip = QString(),
            quint16 port = 0,
            QByteArray key = QByteArray(),
            int id = getNextAvailableID());

    static Contact findById(int id);
    static QList<Contact> findByName(QString name);
    static QList<Contact> findByIp(QString ip);
    static Contact findByKey(QByteArray key);
    static QList<Contact> getContactList();

    static int getNextAvailableID();

    int getId() const;
    QString getName() const;
    QString getIp() const;
    quint16 getPort() const;
    QByteArray getKey() const;

    void setId(int id);
    void setName(QString name);
    void setIp(QString ip);
    void setPort(quint16 port);
    void setKey(QByteArray key);
    void save();
    void erase();

private :
    int mId;
    quint16 mPort;
    QString mName, mIp;
    QByteArray mKey;
};

#endif // CONTACT_H
