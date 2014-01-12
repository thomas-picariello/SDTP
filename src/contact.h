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
            QString port = QString(),
            QByteArray key = QByteArray(),
            int id = getNextAvailableID());

    static Contact findById(int id);
    static Contact findByName(QString name);
    static Contact findByIp(QString ip);
    static Contact findByKey(QByteArray key);
    static QList<Contact> getContactList();

    static int getNextAvailableID();

    QString getId() const;
    QString getName() const;
    QString getIp() const;
    QString getPort() const;
    QByteArray getKey() const;

    void setId(int id);
    void setName(QString name);
    void setIp(QString ip);
    void setPort(QString port);
    void setKey(QByteArray key);
    void save();
    void erase();

private :
    int mId;
    QString mName, mIp, mPort;
    QByteArray mKey;
};

#endif // CONTACT_H
