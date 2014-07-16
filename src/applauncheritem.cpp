#include "applauncheritem.h"

AppLauncherItem::AppLauncherItem()
{
}

AppLauncherItem::AppLauncherItem(AppType appType, QString title, QString iconUrl_normal, QString iconUrl_hover, QString iconUrl_pressed):
    m_appType(appType),
    m_title(title)
{
    m_iconUrls.insert(Normal, iconUrl_normal);
    m_iconUrls.insert(Hover, iconUrl_hover);
    m_iconUrls.insert(Pressed, iconUrl_pressed);
}

AppType AppLauncherItem::getAppType() const{
    return m_appType;
}

QMap<AppLauncherItem::IconState, QString> AppLauncherItem::getIconUrls() const{
    return m_iconUrls;
}

QIcon AppLauncherItem::getIcon() const{
    QIcon icon;
    icon.addFile(m_iconUrls.value(AppLauncherItem::Normal), QSize(), QIcon::Normal, QIcon::On);
    icon.addFile(m_iconUrls.value(AppLauncherItem::Hover), QSize(), QIcon::Active, QIcon::On);
    icon.addFile(m_iconUrls.value(AppLauncherItem::Pressed), QSize(), QIcon::Selected, QIcon::On);
    return icon;
}

QString AppLauncherItem::getTitle() const{
    return m_title;
}

void AppLauncherItem::setAppType(const AppType &appType){
    m_appType = appType;
}

void AppLauncherItem::setIconUrls(QString normal, QString hover, QString pressed){
    m_iconUrls.insert(Normal, normal);
    m_iconUrls.insert(Hover, hover);
    m_iconUrls.insert(Pressed, pressed);
}

void AppLauncherItem::setTitle(const QString &title){
    m_title = title;
}
