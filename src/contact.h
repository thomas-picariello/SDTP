#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QSettings>
#include <QStringList>


class Contact
{
public:
    Contact();
    Contact(QString name);
    Contact(QString name, QString ip, QString port, QByteArray key);

    static QStringList getAllNames();
    static bool nameExists(QString name);
    QString getName() const;
    QString getIp() const;
    QString getPort() const;
    QByteArray getKey() const;

    void setName(QString name);
    void setIp(QString ip);
    void setPort(QString port);
    void setKey(QByteArray key);
    void remove();

    ~Contact();

private :
    QSettings mSettings;
    int mId;
    QString mName, mIp, mPort;
    QByteArray mKey;

    static int getNextAvailableID();
    static int getNameID(QString name);
};

#endif // CONTACT_H
