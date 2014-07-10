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
    //256 bits IV for GCM
    mFileKey.second = salt;

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

    //registerNAT(100,0);

    restartListener();
    connect(&mListener, &QTcpServer::newConnection,
            this, &SVoIP::onNewConnection);
    connect(&mIpFilter, &IpFilter::accepted,
            this, &SVoIP::onIpAccepted);
    connect(mContactListWindow, &ContactListWindow::settingsUpdated,
            this, &SVoIP::restartListener);
    connect(mContactListWindow, &ContactListWindow::contactEvent,
            this, &SVoIP::onContactEvent);
    connect(mContactListWindow, &ContactListWindow::startApp,
            this, &SVoIP::startApp);

    //start a NetworkManager for each contact
    QList<Contact*> contactList = mContactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        NetworkManager* networkManager = new NetworkManager(contact, mContactDB, mRsaKeyring, &mIpFilter, this);
        connectNetworkManagerSignals(networkManager);
        mNetworkManagerList.insert(contact->getId(), networkManager);
    }
}

void SVoIP::onIpAccepted(QTcpSocket *socket){
    //negative unique id for hanshaking network managers
    int id = -1;
    while(mNetworkManagerList.contains(id))
        id--;
    NetworkManager* networkManager = new NetworkManager(socket,mContactDB, mRsaKeyring, &mIpFilter, this);
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
        networkManager = new NetworkManager(mContactDB->findById(id), mContactDB, mRsaKeyring, &mIpFilter, this);
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
    connect(networkManager, &NetworkManager::contactStatusChanged,
            this, &SVoIP::updateContactStatus); //TODO contact self signal
    connect(networkManager, &NetworkManager::destroyed,
            this, &SVoIP::onNetworkManagerDestroy);
    connect(networkManager, &NetworkManager::newContactId,
            this, &SVoIP::updateNetworkManagerId);
    connect(networkManager, &NetworkManager::startApp,
            this, &SVoIP::startApp);
    connect(networkManager, &NetworkManager::startAppFor,
            this, &SVoIP::startAppFor);
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

AbstractApp* SVoIP::startApp(Contact* contact, AppType appType){
    //TODO: see if templated factory may works here...
    //TODO: retrieve the right app for the right contact group
    AbstractApp* app = NULL;
    AppUID appUID(appType);
    if(mAppList.contains(appUID)){
        app = mAppList.value(appUID);
        app->show();
    }else{
        //generate instance id
        foreach(AppUID existingAppUID, mAppList.keys()){
            if(existingAppUID == appUID)
                appUID.setInstanceID(appUID.instanceID() + 1);
        }

        if(appType == Messenger){
            app = new MessengerApp(contact);
        }else{
            emit error(InvalidAppID);
        }
        if(app){
            //register
            mAppList.insert(appUID, app);
            mNetworkManagerList.value(contact->getId())->registerApp(appUID, app);
        }
    }
    return app;
}

AbstractApp* SVoIP::startAppFor(Contact* contact, AppUID distantUID){
    AbstractApp *app = startApp(contact, distantUID.type());
    AppUID localUID = mAppList.key(app);
    if(localUID.type() != Undefined)
        mNetworkManagerList.value(contact->getId())->registerAppConnection(localUID, distantUID);
    return app;
}

void SVoIP::registerNAT(quint16 port,bool connexionType){
    UPNPNAT nat;
    QString error;
    nat.init(15,20);

    QString type = "TCP";
    if(connexionType)type = "UDP";

    if(!nat.discovery()){
        error = nat.get_last_error();
        qDebug()<<"discovery error is : "+error;
    }

    if(!nat.add_port_mapping("svoip","10.207.3.65",port,port,"TCP")){
        error = nat.get_last_error();
        qDebug()<<"add_port_mapping error is : "+error;
    }
    else {
        error = nat.get_last_error();
        qDebug()<<"add port mapping succ.";
    }
}

SVoIP::~SVoIP(){
    if(mContactListWindow) delete mContactListWindow;
    if(mContactDB) delete mContactDB;
    if(mPasswordWindow) delete mPasswordWindow;
    qDeleteAll(mAppList);
    qDeleteAll(QMap<int,NetworkManager*>(mNetworkManagerList)); //copy list to avoid networkManager self remove
}
