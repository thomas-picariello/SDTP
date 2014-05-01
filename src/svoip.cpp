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
    attemptConnectAll();
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

void SVoIP::deleteNetworkManager(NetworkManager *networkManager){
    //disconnect(networkManager, SIGNAL())
    int id = mNetworkManagerList.key(networkManager, 0); //TODO: replace by networkManager->getContact()->getId()
    mNetworkManagerList.remove(id);
    delete networkManager;
}

void SVoIP::updateNetworkManagerId(NetworkManager *networkManager, int newId){
    int oldId = mNetworkManagerList.key(networkManager, 0); //TODO: replace by networkManager->getContact()->getId()
    if(oldId){
        mNetworkManagerList.remove(oldId);
        mNetworkManagerList.insert(newId, networkManager);
    }
}

void SVoIP::updateContactStatus(int contactId, ContactListWindow::Status status){
    mContactListWindow->setContactStatusIcon(contactId, status);
}

void SVoIP::attemptConnectAll(){
    QList<Contact*> contactList = mContactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        int id = contact->getId();
        if(!mNetworkManagerList.contains(id)){
            NetworkManager* networkManager = new NetworkManager(contact, mContactDB, this);
            connectNetworkManagerSignals(networkManager);
            mNetworkManagerList.insert(id, networkManager);
        }
    }
}

void SVoIP::restartListener(){
    mListener.close();
    qint16 port = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();
    mListener.listen(QHostAddress::Any, port);
}

void SVoIP::connectNetworkManagerSignals(NetworkManager *networkManager){
//    connect(networkManager, SIGNAL(),
//            this, SLOT(updateContactStatus(int,ContactListWindow::Status)));
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

SVoIP::~SVoIP(){
    qDeleteAll(mNetworkManagerList);
    if(mContactDB) delete mContactDB;
    if(mContactListWindow) delete mContactListWindow;
    if(mPasswordWindow) delete mPasswordWindow;
}
