#include "svoip.h"

SVoIP::SVoIP(QObject *parent):
    QObject(parent),
    m_contactDB(NULL),
    m_rsaKeyring(NULL),
    m_contactListWindow(NULL),
    m_passwordWindow(NULL)

{
    QSettings settings("settings.ini", QSettings::IniFormat);
    QByteArray salt = QByteArray::fromBase64(settings.value("encryption/salt").toByteArray());
    QByteArray pwdHash = QByteArray::fromBase64(settings.value("encryption/password_hash").toByteArray());


    if(salt.isEmpty()){
        settings.setValue("encryption/salt", generateSalt());
    }
    //256 bits IV for GCM
    m_fileKey.second = salt;

    if(pwdHash.isEmpty()){
        checkParameters();
    }else{
        m_passwordWindow = new PasswordWindow(pwdHash, salt);
        connect(m_passwordWindow, SIGNAL(validate(QByteArray)),
                this, SLOT(checkParameters(QByteArray)));
    }
}

void SVoIP::checkParameters(QByteArray key){
    QSettings settings("settings.ini", QSettings::IniFormat);
    m_fileKey.first = key;
    m_rsaKeyring = new RsaKeyring(&m_fileKey);
    if(!m_rsaKeyring->hasPrivateKey() ||
       settings.value("network/listen_port").isNull()
      ){
        displayConfWizard();
    }else
        startProgram();
}

void SVoIP::displayConfWizard(){
    m_wizard = new ConfWizard(m_fileKey);

    connect(m_wizard,SIGNAL(accepted()),this,SLOT(startProgram()));
    connect(m_wizard,SIGNAL(rejected()),qApp,SLOT(quit()));
}

void SVoIP::startProgram(){
    disconnect(m_rsaKeyring, SIGNAL(privateKeyGenerationFinished(QByteArray)), this, 0);
    m_contactDB = new ContactDB(&m_fileKey, this);
    m_contactListWindow = new ContactListWindow(m_contactDB, m_rsaKeyring, &m_fileKey);

    //registerNAT(100,0);

    restartListener();
    connect(&m_listener, &QTcpServer::newConnection,
            this, &SVoIP::onNewConnection);
    connect(&m_ipFilter, &IpFilter::accepted,
            this, &SVoIP::startHandshaker);
    connect(m_contactListWindow, &ContactListWindow::settingsUpdated,
            this, &SVoIP::restartListener);
    connect(m_contactListWindow, &ContactListWindow::contactEvent,
            this, &SVoIP::onContactEvent);
    connect(m_contactListWindow, &ContactListWindow::startApp,
            this, &SVoIP::startApp);

    //start a Pinger for each contact
    QList<Contact*> contactList = m_contactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        Pinger* pinger = new Pinger(contact);
        m_pingerList.insert(contact, pinger);
        connect(pinger, &Pinger::connected,
                this, &SVoIP::startHandshaker);
        pinger->start();
    }
}

void SVoIP::onNewConnection(){
    m_ipFilter.filter(m_listener.nextPendingConnection());
}

void SVoIP::onNetworkManagerDestroy(NetworkManager* networkManager){
    m_networkManagerList.remove(m_networkManagerList.key(networkManager));
}

void SVoIP::onContactEvent(int id, ContactDB::Event event){
    switch(event){
        case ContactDB::ContactAdded:{
            Pinger* pinger = new Pinger(m_contactDB->findById(id));
            m_pingerList.insert(m_contactDB->findById(id), pinger);
            connect(pinger, &Pinger::connected,
                    this, &SVoIP::startHandshaker);
            pinger->start();
        }
        break;
        case ContactDB::ContactDeleted:{
            if(m_networkManagerList.contains(id))
                delete m_networkManagerList.value(id);
        }
        break;
        case ContactDB::ContactEdited:{
            //not usefull theoretically...

        }
        break;
    }
}

void SVoIP::onHandshakeSuccess(){
    Handshaker* handshaker = dynamic_cast<Handshaker*>(sender());
    if(handshaker){
        handshaker->getContact()->setStatus(Contact::Online);
        QString host = handshaker->getHost();
        m_ipFilter.removeBan(host);
        NetworkManager* netMgr = new NetworkManager(handshaker->getContact(),
                                                    handshaker->getSocket(),
                                                    handshaker->getGcmKey(),
                                                    handshaker->getGcmBaseIV(),
                                                    this);
        if(host == "127.0.0.1" && handshaker->getMode() == Handshaker::ResponderMode)
            m_networkManagerList.insert(0, netMgr);     //TODO: remove localhost debug
        else
            m_networkManagerList.insert(handshaker->getContact()->getId(), netMgr);
        connect(netMgr, &NetworkManager::destroyed,
                this, &SVoIP::onNetworkManagerDestroy);
        connect(netMgr, &NetworkManager::startApp,
                this, &SVoIP::startApp);
        connect(netMgr, &NetworkManager::startAppFor,
                this, &SVoIP::startAppFor);
        delete handshaker;
        m_handshakerList.remove(host);
    }
}

void SVoIP::onHandshakeError(Handshaker::Error error){
    Handshaker* handshaker = dynamic_cast<Handshaker*>(sender());
    if(handshaker){
        qDebug()<< handshaker->getErrorString(error);
        QString host = handshaker->getHost();
        if(handshaker->getRecievedBanTime() != 0)
            m_ipFilter.addBan(host, handshaker->getRecievedBanTime());
        Contact* contact = handshaker->getContact();
        if(contact){
            const quint16 banTime = m_ipFilter.getRemainingBanTime(host);
            m_pingerList.value(contact)->start(banTime);
        }
        delete handshaker;
        m_handshakerList.remove(host);
    }

}

void SVoIP::restartListener(){
    m_listener.close();
    qint16 port = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();
    m_listener.listen(QHostAddress::Any, port);
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
    if(m_appList.contains(appUID) && contact->getHostsList().contains("127.0.0.1")){ //TODO: remove localhost debug
        app = m_appList.value(appUID);
        app->show();
    }else{
        //generate instance id
        foreach(AppUID existingAppUID, m_appList.keys()){
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
            m_appList.insert(appUID, app);
            m_networkManagerList.value(contact->getId())->registerApp(appUID, app);
        }
    }
    return app;
}

AbstractApp* SVoIP::startAppFor(Contact* contact, AppUID distantUID){
    AbstractApp *app = startApp(contact, distantUID.type());
    AppUID localUID = m_appList.key(app);
    if(localUID.type() != Undefined){
        NetworkManager* netMgr = m_networkManagerList.value(contact->getId());
        netMgr->registerAppConnection(localUID, distantUID);
        netMgr->onAppStarted(localUID);
    }
    return app;
}

void SVoIP::startHandshaker(QTcpSocket* socket){
    Pinger* pinger = dynamic_cast<Pinger*>(sender());
    IpFilter* filter = dynamic_cast<IpFilter*>(sender());
    if(pinger || filter){
        QString host = socket->peerAddress().toString();
        if(getHostState(host) != NotConnected && host != "127.0.0.1")   //TODO: remove debug localhost
            socket->close();
        else{
            Handshaker* handshaker = new Handshaker(socket, m_rsaKeyring, this);
            m_handshakerList.insertMulti(socket->peerAddress().toString(), handshaker); //TODO: debug multi
            connect(handshaker, &Handshaker::success, this, &SVoIP::onHandshakeSuccess);
            connect(handshaker, &Handshaker::error, this, &SVoIP::onHandshakeError);
            if(pinger)
                handshaker->startHandshake(pinger->getContact());
            else
                handshaker->waitForHandshake(m_contactDB);
        }
    }
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

SVoIP::IpState SVoIP::getHostState(QString host){
    foreach(NetworkManager* netMgr, m_networkManagerList) {
        if(netMgr->getHost() == host)
            return Connected;
    }
    if(m_handshakerList.contains(host)){
        return Handshaking;
    }else
        return NotConnected;
}

SVoIP::~SVoIP(){
    if(m_contactListWindow) delete m_contactListWindow;
    if(m_contactDB) delete m_contactDB;
    if(m_passwordWindow) delete m_passwordWindow;
    qDeleteAll(m_appList);
    qDeleteAll(QMap<int,NetworkManager*>(m_networkManagerList)); //copy list to avoid networkManager self remove
}
