#ifndef CONTACT_H
#define CONTACT_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QLayout>
#include <QSettings>
#include <QList>


class Contact
{
public:
    Contact();
    ~Contact();

public slots :
    QList<QString> getnames();
    QString getIP(QString);
    QString getport(QString);
    QByteArray getkey(QString );

    void setname(QString);
    void setIP(QString , QString);
    void setport(QString, QString);
    void setkey(QString, QByteArray);

private :
    QSettings *settings;
    QList<QString> *names;
    QString *name, *IP, *port;
    QByteArray *key;





};

#endif // CONTACT_H
