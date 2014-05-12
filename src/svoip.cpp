#include "svoip.h"

SVoIP::SVoIP(QObject *parent):
    QObject(parent),
    mContactDB(NULL),
    mContactListWindow(NULL),
    mPasswordWindow(NULL)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    QString salt = settings.value("encryption/salt").toString();
    QString pwdHash = settings.value("encryption/password_hash").toString();

    if(salt.isEmpty()){
        settings.setValue("encryption/salt", generateSalt());
    }
    //set IV (AES block size = 128 bits)
    mFileKey.second = QByteArray::fromBase64(salt.toUtf8()).left(16);

    if(pwdHash.isEmpty()){
        startProgram();
    }else{
        mPasswordWindow = new PasswordWindow(pwdHash, salt);
        connect(mPasswordWindow, SIGNAL(validate(QByteArray)),
                this, SLOT(startProgram(QByteArray)));
    }


}

void SVoIP::startProgram(QByteArray key){
    mFileKey.first = key;
    mContactDB = new ContactDB(&mFileKey, this);
    mContactListWindow = new ContactListWindow(mContactDB, &mFileKey);
    restartListener();
    connect(&mListener, SIGNAL(newConnection()),
            this, SLOT(onIncommingConnection()));
    connect(mContactListWindow, SIGNAL(settingsUpdated()),
            this, SLOT(restartListener()));
    connect(mContactListWindow, SIGNAL(contactEvent(int,Contact::Event)),
            this, SLOT(onContactEvent(int,Contact::Event)));
    connect(mContactListWindow, SIGNAL(startApp(int,int)),
            this,SLOT(onStartAppRequest(int,int)));

    //start a NetworkManager for each contact
    QList<Contact*> contactList = mContactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        NetworkManager* networkManager = new NetworkManager(contact, mContactDB, this);
        connectNetworkManagerSignals(networkManager);
        mNetworkManagerList.insert(contact->getId(), networkManager);
    }


}

void SVoIP::onIncommingConnection(){
    //negative unique id for hanshaking network managers
    int id = -1;
    while(mNetworkManagerList.contains(id))
        id--;
    NetworkManager* networkManager = new NetworkManager(mListener.nextPendingConnection(),mContactDB, this);
    connectNetworkManagerSignals(networkManager);
    mNetworkManagerList.insert(id, networkManager);


}

void SVoIP::onNetworkManagerDelete(QObject *object){
    int id = mNetworkManagerList.key(static_cast<NetworkManager*>(object), 0);
    mNetworkManagerList.remove(id);
}

void SVoIP::updateNetworkManagerId(NetworkManager *networkManager, int newId){
    int oldId = mNetworkManagerList.key(networkManager, 0);
    if(oldId){
        mNetworkManagerList.remove(oldId);
        mNetworkManagerList.insert(newId, networkManager);
    }
}

void SVoIP::updateContactStatus(int id, Contact::Status status){
    if(id)
        mContactListWindow->setContactStatusIcon(id, status);
}

void SVoIP::onContactEvent(int id, Contact::Event event){
    NetworkManager* networkManager;
    switch(event){
    case Contact::Added:
        networkManager = new NetworkManager(mContactDB->findById(id), mContactDB, this);
        connectNetworkManagerSignals(networkManager);
        mNetworkManagerList.insert(id, networkManager);
        break;
    case Contact::Deleted:
        delete mNetworkManagerList.value(id);
        break;
    case Contact::Updated:
        mNetworkManagerList.value(id)->onContactEvent(event);
        break;
    }
}

void SVoIP::restartListener(){
    mListener.close();
    qint16 port = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();
    mListener.listen(QHostAddress::Any, port);
}

void SVoIP::connectNetworkManagerSignals(NetworkManager *networkManager){
    connect(networkManager, SIGNAL(statusChanged(int, Contact::Status)),
            this, SLOT(updateContactStatus(int, Contact::Status)));
    connect(networkManager, SIGNAL(destroyed(QObject*)),
            this, SLOT(onNetworkManagerDelete(QObject*)));
}

QString SVoIP::generateSalt(){
    const unsigned int blockSize = 32;
    byte randomBlock[blockSize];
    std::string encodedBlock;
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(randomBlock,blockSize);
    CryptoPP::ArraySource(randomBlock,
                          blockSize,
                          true,
                          new CryptoPP::Base64Encoder(
                              new StringSink(encodedBlock),
                              false));
    return QString::fromStdString(encodedBlock);
}
void SVoIP::onStartAppRequest(int appId, int contactId){

    qDebug()<<"startAppRequest";
}

SVoIP::~SVoIP(){
    if(mContactDB) delete mContactDB;
    if(mContactListWindow) delete mContactListWindow;
    if(mPasswordWindow) delete mPasswordWindow;
}
