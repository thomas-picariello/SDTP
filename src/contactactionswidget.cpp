#include "contactactionswidget.h"

ContactActionsWidget::ContactActionsWidget(int contactId, QWidget *parent) :
    QWidget(parent),
    m_contactId(contactId),
    m_appLauchersHidden(true)
{
    m_optionsMenu = new QMenu(this);

    setStyleSheet(/*Global*/
                  "QWidget{"
                      "background-color:transparent;"
                  "}"
                  /*QPushButton*/
                  "QPushButton{"
                      "background-color:transparent;"
                      "border:none;"
                      "padding: 0 0;"
                      "margin:0;"
                  "}"
                  /*QMenu*/
                  "QMenu{"
                      "background-color:white;"
                      "padding: 0;"
                      "margin: 0;"
                  "}"
                  "QMenu::item{"
                      "padding: 5px 5px 5px 36px;"
                  "}"
                  "QMenu::item::selected{"
                      "color:white;"
                      "background-color:#09f;"
                  "}"
                  "QMenu::item::pressed{"
                      "color:white;"
                      "background-color:#07f;"
                  "}");
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(0,0,6,0));
    mainLayout->addSpacerItem(new QSpacerItem(40,20,QSizePolicy::Expanding));

    mainLayout->addLayout(&m_appLauncherButtonsLayout);

    m_optionsMenuButton = new QPushButton();
    m_optionsMenuButton->setStyleSheet("QPushButton{"
                           "image: url(\":/icons/options_normal\");"
                      "}"
                      "QPushButton:hover{"
                           "image: url(\":/icons/options_hover\");"
                      "}"
                      "QPushButton:pressed{"
                          "image: url(\":/icons/options_pressed\");"
                      "}");
    m_optionsMenuButton->setToolTip(tr("more..."));
    m_optionsMenuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_optionsMenuButton->setMinimumSize(22,22);
    connect(m_optionsMenuButton, &QPushButton::clicked, this, &ContactActionsWidget::showMenu);
    mainLayout->addWidget(m_optionsMenuButton);

    setLayout(mainLayout);
    buildAppLaunchers();
    buildContactActions();
    hide();
}

int ContactActionsWidget::getContactId() const{
    return m_contactId;
}


void ContactActionsWidget::addAppLauncherItem(AppLauncherItem *item){
    m_appLauncherList.append(item);
    buildAppLaunchers();
    buildContactActions();
}

void ContactActionsWidget::addAppLauncherItem(int row, AppLauncherItem *item){
    m_appLauncherList.insert(row, item);
    buildAppLaunchers();
    buildContactActions();
}

void ContactActionsWidget::setContactId(int id){
    m_contactId = id;
}

void ContactActionsWidget::setAppLauncherHidden(bool hidden){
    m_appLauchersHidden = hidden;
}

void ContactActionsWidget::hide(){
    foreach(QPushButton* bt, m_appLauncherButtonsList)
        bt->hide();
    foreach(QAction* action, m_actionList)
        action->setVisible(false);
    m_optionsMenuButton->hide();
}

void ContactActionsWidget::show(){
    m_optionsMenuButton->show();
    if(!m_appLauchersHidden){
        foreach(QPushButton* bt, m_appLauncherButtonsList)
            bt->show();
        foreach(QAction* action, m_actionList)
            action->setVisible(true);
    }
}

void ContactActionsWidget::onLauncherClick(){
    QPushButton* bt = dynamic_cast<QPushButton*>(sender());
    QAction* action = dynamic_cast<QAction*>(sender());
    if(bt || action){
        AppType type;
        if(bt)
            type = static_cast<AppType>(bt->property("AppType").toUInt());
        else
            type = static_cast<AppType>(action->data().toUInt());
        emit startApp(m_contactId, type);
    }
}

void ContactActionsWidget::onEditAction(){
    emit contactAction(m_contactId, ContactDB::Edit);
}

void ContactActionsWidget::onDeleteAction(){
    emit contactAction(m_contactId, ContactDB::Delete);
}

void ContactActionsWidget::showMenu(){
    QPushButton *bt = dynamic_cast<QPushButton*>(sender());
    if(bt){
        QPoint pos = bt->geometry().bottomLeft();
        pos = bt->mapFromParent(pos);
        pos = bt->mapToGlobal(pos);
        m_optionsMenu->exec(pos);
    }
}

void ContactActionsWidget::buildAppLaunchers()
{
    //clean layout
    QLayoutItem *item;
    while((item = m_appLauncherButtonsLayout.takeAt(0)) != 0){
        delete item->widget();
        delete item;
    }
    m_appLauncherButtonsList.clear();

    //add app launchers and the menu bt
    for(int i=0; (i<MAX_FAVORITE_ICONS && i<m_appLauncherList.size()); i++){
        QPushButton *bt = new QPushButton();
        QString normalIconUrl = m_appLauncherList.at(i)->getIconUrls().value(AppLauncherItem::Normal);
        QString hoverIconUrl = m_appLauncherList.at(i)->getIconUrls().value(AppLauncherItem::Hover);
        QString pressedIconUrl = m_appLauncherList.at(i)->getIconUrls().value(AppLauncherItem::Pressed);
        QString stylesheet = "QPushButton{"
                                "image: url(\""+ normalIconUrl +"\");"
                             "}";
        if(!hoverIconUrl.isEmpty())
            stylesheet += "QPushButton:hover{"
                              "image: url(\""+ hoverIconUrl +"\");"
                          "}";
        if(!pressedIconUrl.isEmpty())
            stylesheet += "QPushButton:pressed{"
                              "image: url(\""+ pressedIconUrl +"\");"
                          "}";
        bt->setStyleSheet(stylesheet);
        bt->setToolTip(m_appLauncherList.at(i)->getTitle());
        bt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        bt->setMinimumSize(22,22);
        bt->setProperty("AppType", m_appLauncherList.at(i)->getAppType());
        bt->hide();
        connect(bt, &QPushButton::clicked, this, &ContactActionsWidget::onLauncherClick);

        m_appLauncherButtonsList.append(bt);
        m_appLauncherButtonsLayout.addWidget(bt);
    }
}

void ContactActionsWidget::buildContactActions()
{
    m_optionsMenu->clear();
    m_actionList.clear();
    //add the rest of the app launchers
    for(int i=MAX_FAVORITE_ICONS; i<m_appLauncherList.size(); i++){
        QAction *menuItem = new QAction(m_appLauncherList.at(i)->getIcon(),
                                        m_appLauncherList.at(i)->getTitle(),
                                        m_optionsMenu);
        m_actionList.append(menuItem);
        m_optionsMenu->addAction(menuItem);
        connect(menuItem, &QAction::triggered, this, &ContactActionsWidget::onLauncherClick);
    }

    m_optionsMenu->addSeparator();

    QAction *editAction = new QAction(getIcon(":/icons/edit_normal",":/icons/edit_hover",":/icons/edit_pressed"),
                                      tr("&Edit"),
                                      m_optionsMenu);
    m_optionsMenu->addAction(editAction);
    connect(editAction, &QAction::triggered, this, &ContactActionsWidget::onEditAction);

    QAction *deleteAction = new QAction(getIcon(":/icons/delete_normal",":/icons/delete_hover",":/icons/delete_pressed"),
                                      tr("&Delete"),
                                      m_optionsMenu);
    m_optionsMenu->addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, &ContactActionsWidget::onDeleteAction);
}

QIcon ContactActionsWidget::getIcon(QString normal, QString hover, QString pressed, QSize size){
    QIcon icon;
    icon.addFile(normal, size, QIcon::Normal, QIcon::On);
    icon.addFile(hover, size, QIcon::Active, QIcon::On);
    icon.addFile(pressed, size, QIcon::Selected, QIcon::On);
    return icon;
}

ContactActionsWidget::~ContactActionsWidget(){
    delete m_optionsMenuButton;
    delete m_optionsMenu;
    qDeleteAll(m_appLauncherList);
}

