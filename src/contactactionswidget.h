/**
 * @brief The GUI part of an applauncherItem. Which is used to Launch an manipulate apps
 */

#ifndef CONTACTACTIONSWIDGET_H
#define CONTACTACTIONSWIDGET_H

//#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QMenu>

#include "typesenums.h"
#include "contactdb.h"
#include "applauncheritem.h"

class ContactActionsWidget : public QWidget
{
    Q_OBJECT
public:
    static const quint8 MAX_FAVORITE_ICONS = 3;

    explicit ContactActionsWidget(int contactId = 0,
                               QWidget *parent = 0);
    ~ContactActionsWidget();

    void addAppLauncherItem(AppLauncherItem *item);
    void addAppLauncherItem(int row, AppLauncherItem *item);
    int getContactId() const;
    void setContactId(int id);
    void setAppLauncherHidden(bool hidden);

signals:
    void contactAction(uint contactId, ContactDB::Action action);
    void startApp(uint contactId, AppType appType);

public slots:
    void hide();
    void show();

private slots:
    void onLauncherClick();
    void onEditAction();
    void onDeleteAction();
    void showMenu();

private:
    bool m_appLauchersHidden;
    QHBoxLayout m_appLauncherButtonsLayout;
    QList<AppLauncherItem*> m_appLauncherList;
    QList<QPushButton*> m_appLauncherButtonsList;
    QList<QAction*> m_actionList;
    int m_contactId;
    QPushButton* m_optionsMenuButton;
    QMenu *m_optionsMenu;

    void buildAppLaunchers();
    void buildContactActions();
    QIcon getIcon(QString normal, QString hover, QString pressed, QSize size = QSize());
};

#endif // CONTACTACTIONSWIDGET_H
