#ifndef APPLAUNCHERITEM_H
#define APPLAUNCHERITEM_H

#include <QIcon>
#include <QMap>

#include "typesenums.h"

class AppLauncherItem
{
public:
    enum IconState{
        Normal,
        Hover,
        Pressed
    };

    AppLauncherItem();
    AppLauncherItem(AppType appType, QMap<AppLauncherItem::IconState, QString> &iconUrls, QString &title);

    AppType getAppType() const;
    QMap<IconState, QString> getIconUrls() const;
    QIcon getIcon() const;
    QString getTitle() const;
    void setAppType(const AppType &appType);
    void setIconUrls(QString normal, QString hover = QString(), QString pressed = QString());
    void setTitle(const QString &title);

private:
    AppType m_appType;
    QMap<IconState, QString> m_iconUrls;
    QString m_title;

};

#endif // APPLAUNCHERITEM_H
