#ifndef SVOIP_H
#define SVOIP_H

#include <QObject>
#include <QByteArray>
#include <QPair>
#include <QMessageBox>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>
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
    void onPasswordInput(QString password);

signals:
    void error(QString err);

private :
    QString mPwHash, mSalt;
    ContactDB mContactDB;
    PasswordWindow mPasswordWindow;
    ContactListWindow *mContactListWindow;
    QPair<QByteArray,QByteArray> mFileKey;

    QByteArray deriveKey(QString password);
    QByteArray generateSalt();

    Q_DISABLE_COPY(SVoIP)
};

#endif // SVOIP_H
