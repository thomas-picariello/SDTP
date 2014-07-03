#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QMap>

#include "typesenums.h"
#include "abstractapp.h"
#include "appuid.h"

class AppManager : public QObject
{
    Q_OBJECT
public:
    explicit AppManager(QObject* parent = 0);
    ~AppManager();

public slots:
    void readIncommingData(QByteArray &data);
    void startApp(AppType appType);
    void requestPartnerApp(AppType);
    void registerApp(AppUID localAppUID, AbstractApp *app);
    void unregisterApp(AppUID localAppUID);

signals:
    void sendData(QByteArray& data);

private:
    QMap<AppUID, AppUID> m_AppConnectionTable;
};

#endif // APPMANAGER_H
