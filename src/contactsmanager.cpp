#include "contactsmanager.h"

ContactsManager::ContactsManager()
{

    m_contactManagerMainwindow = new QWidget();
    m_addContactWindow = new QWidget();
    m_editContactWindow = new QWidget();

    m_settings = new QSettings;

    m_contact = new Contact();

    m_addContactButton = new QPushButton("Add+");
    m_editContactButton = new QPushButton("Edit_");
    m_saveContactButton = new QPushButton("Save changes");

    menu = new QMenu();

    m_IPLineEdit = new QLineEdit;
    m_portLineEdit = new QLineEdit;
    m_keyLineEdit = new QLineEdit;
    m_nameLineEdit = new QLineEdit;

    m_addAndEditButtonLayout = new QVBoxLayout;
    m_contactManagerWindowMainLayout = new QHBoxLayout;
    m_addWindowMainLayout = new QVBoxLayout;
    m_addWindowCenterLayout = new QHBoxLayout;
    m_addWindowLabelLayout = new QVBoxLayout;
    m_addWindowLineEditLayout = new QVBoxLayout;

    m_contactListLabel = new QLabel;
    m_addlabel = new QLabel;
    m_IPlabel = new QLabel;
    m_portlabel = new QLabel;
    m_keylabel = new QLabel;
    m_namelabel = new QLabel;

    menu->addAction(" /Under Construction\\");
    menu->addSeparator();
    menu->addAction("Just use Add function instead.");

    m_editContactButton->setMenu(menu);

    m_portLineEdit->setInputMask("00009");
    m_portLineEdit->setMaxLength(5);
    m_IPLineEdit->setInputMask("009.009.009.009");
    m_nameLineEdit->setMaxLength(10);
    m_nameLineEdit->setInputMask("nnnnnnnnnn");

    m_contactListLabel->setText("text to test the text capacity of QLabel");
    m_addlabel->setText("New Contact informations");
    m_IPlabel->setText("IP :");
    m_portlabel->setText("port :");
    m_namelabel->setText("name :");
    m_keylabel->setText("key :");






    m_addWindowLineEditLayout->addWidget(m_nameLineEdit);
    m_addWindowLineEditLayout->addWidget(m_IPLineEdit);
    m_addWindowLineEditLayout->addWidget(m_portLineEdit);
    m_addWindowLineEditLayout->addWidget(m_keyLineEdit);

    m_addWindowLabelLayout->addWidget(m_namelabel);
    m_addWindowLabelLayout->addWidget(m_IPlabel);
    m_addWindowLabelLayout->addWidget(m_portlabel);
    m_addWindowLabelLayout->addWidget(m_keylabel);

    m_addWindowCenterLayout->addLayout(m_addWindowLabelLayout);
    m_addWindowCenterLayout->addLayout(m_addWindowLineEditLayout);

    m_addWindowMainLayout->addWidget(m_addlabel);
    m_addWindowMainLayout->addLayout(m_addWindowCenterLayout);
    m_addWindowMainLayout->addWidget(m_saveContactButton);



    m_addAndEditButtonLayout->addWidget(m_addContactButton);
    m_addAndEditButtonLayout->addWidget(m_editContactButton);

    m_contactManagerWindowMainLayout->addLayout(m_addAndEditButtonLayout);
    m_contactManagerWindowMainLayout->addWidget(m_contactListLabel);/////////////<---

    m_contactManagerMainwindow->setLayout(m_contactManagerWindowMainLayout);
    m_addContactWindow->setLayout(m_addWindowMainLayout);




    connect(m_addContactButton  ,SIGNAL(clicked()),this,SLOT(addcontact()));
    connect(m_editContactButton ,SIGNAL(clicked()),this,SLOT(editcontact()));
    connect(m_saveContactButton ,SIGNAL(clicked()),this,SLOT(savecontact()));





}
void ContactsManager::contactsWindow()
{
    qDebug()<<"contactsWindow !";
    m_settings->beginGroup("Contacts/");
    qDebug()<<m_settings->childGroups();

    QStringList list;

    list = m_settings->childGroups();

    m_contactListLabel->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));

    m_settings->endGroup();
    m_contactManagerMainwindow->show();


}
void ContactsManager::addcontact()
{
    m_nameLineEdit->setPlaceholderText("Name :");
    m_portLineEdit->setPlaceholderText("80");
    m_IPLineEdit->setPlaceholderText("127.0.0.1");
    m_keyLineEdit->setPlaceholderText("QWEQJBKQEQGE");

    qDebug()<<"m_addContactWindow";
    m_addContactWindow->show();





}
void ContactsManager::editcontact()
{


    m_addlabel->setText("Edit Contact informations");


    QList<QString> namelist;
    namelist = m_contact->getnames();




    m_editContactWindow->setLayout(m_addWindowMainLayout);
    qDebug()<<"m_editContactWindow";
    m_editContactWindow->show();



}
void ContactsManager::savecontact()
{
    m_addContactWindow->hide();

    QStringList list;

    m_contact->setname(m_nameLineEdit->text());
    m_contact->setIP(m_nameLineEdit->text(),m_IPLineEdit->text());
    m_contact->setport(m_nameLineEdit->text(),m_portLineEdit->text());
    m_contact->setkey(m_nameLineEdit->text(),m_keyLineEdit->text().toUtf8());

    list = m_contact->getnames();
    m_contactListLabel->setText("List of existing and used contact names :\n\n - "+list.join("\n - "));

    m_nameLineEdit->clear();
    m_portLineEdit->clear();
    m_IPLineEdit->clear();
    m_keyLineEdit->clear();


}

ContactsManager::~ContactsManager(void)
{

}
