#include "svoip.h"

SVoIP::SVoIP(QObject *parent):
    QObject(parent),
    mContactDB(NULL),
    mRsaKeyring(NULL),
    mContactListWindow(NULL),
    mPasswordWindow(NULL)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    QByteArray salt = QByteArray::fromBase64(settings.value("encryption/salt").toByteArray());
    QByteArray pwdHash = QByteArray::fromBase64(settings.value("encryption/password_hash").toByteArray());

    if(salt.isEmpty()){
        settings.setValue("encryption/salt", generateSalt());
    }
    //set IV (AES block size = 128 bits) TODO: update for GCM (filekey)
    mFileKey.second = salt.left(16);

    if(pwdHash.isEmpty()){
        checkParameters();
    }else{
        mPasswordWindow = new PasswordWindow(pwdHash, salt);
        connect(mPasswordWindow, SIGNAL(validate(QByteArray)),
                this, SLOT(checkParameters(QByteArray)));
    }
}

void SVoIP::checkParameters(QByteArray key){
    QSettings settings("settings.ini", QSettings::IniFormat);
    mFileKey.first = key;
    mRsaKeyring = new RsaKeyring(&mFileKey);
    if(!mRsaKeyring->hasPrivateKey() ||
       settings.value("network/listen_port").isNull()
      ){
        displayFirstStartWizard();
    }else
        startProgram();
}

void SVoIP::displayFirstStartWizard(){


    m_wizard = new ConfWizard(mFileKey);


    connect(m_wizard,SIGNAL(accepted()),this,SLOT(startProgram()));
    connect(m_wizard,SIGNAL(rejected()),qApp,SLOT(quit()));

}

void SVoIP::startProgram(){
    disconnect(mRsaKeyring, SIGNAL(privateKeyGenerationFinished(QByteArray)), this, 0);
    mContactDB = new ContactDB(&mFileKey, this);
    mContactListWindow = new ContactListWindow(mContactDB, mRsaKeyring, &mFileKey);

    restartListener();
    connect(&mListener, SIGNAL(newConnection()),
            this, SLOT(onNewConnection()));
    connect(&mIpFilter, SIGNAL(accepted(QTcpSocket*)),
            this, SLOT(onIpAccepted(QTcpSocket*)));
    connect(mContactListWindow, SIGNAL(settingsUpdated()),
            this, SLOT(restartListener()));
    connect(mContactListWindow, SIGNAL(contactEvent(int,Contact::Event)),
            this, SLOT(onContactEvent(int,Contact::Event)));
    connect(mContactListWindow, SIGNAL(startApp(int,AppType)),
            this,SLOT(startApp(int,AppType)));

    //start a NetworkManager for each contact
    QList<Contact*> contactList = mContactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        NetworkManager* networkManager = new NetworkManager(contact, mContactDB, mRsaKeyring, this);
        connectNetworkManagerSignals(networkManager);
        mNetworkManagerList.insert(contact->getId(), networkManager);
    }
}

void SVoIP::onIpAccepted(QTcpSocket *socket){
    //negative unique id for hanshaking network managers
    int id = -1;
    while(mNetworkManagerList.contains(id))
        id--;
    NetworkManager* networkManager = new NetworkManager(socket,mContactDB, mRsaKeyring, this);
    connectNetworkManagerSignals(networkManager);
    mNetworkManagerList.insert(id, networkManager);
}

void SVoIP::onNewConnection(){
    mIpFilter.filter(mListener.nextPendingConnection());
}

void SVoIP::onNetworkManagerDestroy(NetworkManager* networkManager){
        mNetworkManagerList.remove(mNetworkManagerList.key(networkManager));
}

void SVoIP::updateNetworkManagerId(int newId){
    NetworkManager *networkManager = dynamic_cast<NetworkManager*>(sender());
    if(networkManager){
        int oldId = mNetworkManagerList.key(networkManager, 0);
        if(oldId){
            mNetworkManagerList.remove(oldId);
            mNetworkManagerList.insert(newId, networkManager);
        }
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
        networkManager = new NetworkManager(mContactDB->findById(id), mContactDB, mRsaKeyring, this);
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
    connect(networkManager, SIGNAL(contactStatusChanged(int,Contact::Status)),
            this, SLOT(updateContactStatus(int, Contact::Status)));
    connect(networkManager, SIGNAL(destroyed(NetworkManager*)),
            this, SLOT(onNetworkManagerDestroy(NetworkManager*)));
    connect(networkManager, SIGNAL(newContactId(int)),
            this, SLOT(updateNetworkManagerId(int)));
//    connect(networkManager,SIGNAL(startAppRequest(int,AppType)),
//            this,SLOT(startApp(int,AppType)));
    //TODO:root app emit this
}

QString SVoIP::generateSalt(){
    const uint blockSize = 32;
    byte randomBlock[blockSize];
    std::string encodedBlock;
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(randomBlock,blockSize);
    CryptoPP::ArraySource(randomBlock,
                          blockSize,
                          true,
                          new CryptoPP::Base64Encoder(
                              new CryptoPP::StringSink(encodedBlock),
                              false));
    return QString::fromStdString(encodedBlock);
}

void SVoIP::startApp(int contactId, AppType appType){
    //TODO: see if templated factory works here...
    QList<Contact*> contactList;
    QPair<int, AbstractApp::AppUID> key(contactId, AbstractApp::AppUID(appType));
    if(mNetworkManagerList.contains(contactId)){
        if(mAppList.contains(key)){
            mAppList.value(key)->show();
        }else{
            AbstractApp *app = 0;
            contactList.append(mContactDB->findById(contactId));
            if(appType == Root){
                //app = new RootApp(); //TODO: revise RootApp constructor
            }else if(appType == Messenger){
                app = new MessengerApp(contactList, mContactDB);
            }else{
                emit error(tr("Invalid appId"));
            }
            if(app != 0){
                key.second.instanceID = 0; //TODO: generate instance id
                mNetworkManagerList.value(contactId)->registerApp(key.second, app);
                mAppList.insert(key, app);
            }
        }
    }else
        emit error(tr("Invalid contactId"));
}

SVoIP::~SVoIP(){
    if(mContactListWindow) delete mContactListWindow;
    if(mContactDB) delete mContactDB;
    if(mPasswordWindow) delete mPasswordWindow;
    qDeleteAll(mAppList);
    qDeleteAll(QMap<int,NetworkManager*>(mNetworkManagerList)); //copy list
}
