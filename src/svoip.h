#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include <QByteArray>
#include <QPair>
#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>
#include "contactdb.h"
#include "contactlistwindow.h"
#include "passwordwindow.h"

class SVoIP: public QObject
{
    Q_OBJECT
public: 
	SVoIP(QObject *parent = 0);
    ~SVoIP();

public slots:
    void startProgram(QByteArray key = QByteArray());

signals:
    void error(QString err);

private :
    ContactDB *mContactDB;
    PasswordWindow *mPasswordWindow;
    ContactListWindow *mContactListWindow;
    QPair<QByteArray,QByteArray> mFileKey;

    QString generateSalt();

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
