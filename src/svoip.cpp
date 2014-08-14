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
        connect(m_passwordWindow, &PasswordWindow::validate,
                this, &SVoIP::checkParameters);
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
    m_wizard = new ConfWizard(&m_fileKey, m_rsaKeyring);

    connect(m_wizard,SIGNAL(accepted()),this,SLOT(startProgram()));
    connect(m_wizard,SIGNAL(rejected()),qApp,SLOT(quit()));
}

void SVoIP::startProgram(){
    disconnect(m_rsaKeyring, &RsaKeyring::privateKeyGenerationFinished, this, 0);
    m_contactDB = new ContactDB(&m_fileKey, this);
    m_contactListWindow = new ContactListWindow(m_contactDB, m_rsaKeyring, &m_fileKey);

    //registerNAT(100,0);

    restartListener();
    connect(&m_listener, &QTcpServer::newConnection,
            this, &SVoIP::onNewConnection);
    connect(&m_ipFilter, &IpFilter::accepted,
            this, &SVoIP::startHandshake);
    connect(m_contactListWindow, &ContactListWindow::settingsUpdated,
            this, &SVoIP::restartListener);
    connect(m_contactDB, &ContactDB::contactEvent,
            this, &SVoIP::onContactEvent);
    connect(m_contactListWindow, &ContactListWindow::startApp,
            this, &SVoIP::onStartAppRequest);

    //start a Pinger for each contact
    QList<Contact*> contactList = m_contactDB->getAllContacts();
    foreach(Contact *contact, contactList){
        Pinger* pinger = new Pinger(contact);
        m_pingerList.insert(contact, pinger);
        connect(pinger, &Pinger::connected,
                this, &SVoIP::startHandshake);
        pinger->start();
    }
}

void SVoIP::onNewConnection(){
    m_ipFilter.filter(m_listener.nextPendingConnection());
}

void SVoIP::onContactEvent(int id, ContactDB::Event event){
    Contact* contact = m_contactDB->findById(id);
    switch(event){
        case ContactDB::ContactAdded:{
            Pinger* pinger = new Pinger(contact);
            m_pingerList.insert(contact, pinger);
            connect(pinger, &Pinger::connected,
                    this, &SVoIP::startHandshake);
            pinger->start();
        }
        break;
        case ContactDB::ContactDeleted:{
            NetworkManager* netMgr = m_networkManagerList.value(contact, NULL);
            if(netMgr) delete netMgr;
        }
        break;
    }
}

void SVoIP::onDisconnect(Contact* contact){
    foreach(AppUID uid, m_appRegisterTable.keys(contact)){
        m_appRegisterTable.remove(uid);
        m_appList.value(uid)->deleteLater();
        m_appList.remove(uid);
    }
    m_networkManagerList.value(contact)->deleteLater();
    m_networkManagerList.remove(contact);

    Pinger *pinger = m_pingerList.value(contact, NULL);
    if(pinger)
        pinger->start();
}

void SVoIP::onHandshakeSuccess(){
    AbstractHandshake* handshake = dynamic_cast<AbstractHandshake*>(sender());
    if(handshake){
        Contact* contact = handshake->getContact();
        contact->setStatus(Contact::Online);
        QString host = handshake->getHost();
        m_ipFilter.removeBan(host);
        if(!m_networkManagerList.contains(contact)){
            NetworkManager* netMgr = new NetworkManager(handshake->getContact(),
                                                        handshake->getSocket(),
                                                        handshake->getGcmKey(),
                                                        handshake->getGcmBaseIV(),
                                                        this);
            m_networkManagerList.insert(contact, netMgr);
            connect(netMgr, &NetworkManager::disconnected,
                    this, &SVoIP::onDisconnect);
            connect(netMgr, &NetworkManager::startAppFor,
                    this, &SVoIP::onStartAppForRequest);
        }
        delete handshake;
        m_handshakeList.remove(host);
    }
}

void SVoIP::onHandshakeError(AbstractHandshake::Error error){
    AbstractHandshake* handshake = dynamic_cast<AbstractHandshake*>(sender());
    if(handshake){
        //qDebug()<< "Handshake error:" << handshake->getErrorString(error);
        QString host = handshake->getHost();
        if(handshake->getRecievedBanTime() != 0)
            m_ipFilter.addBan(host, handshake->getRecievedBanTime());

        const quint16 banTime = m_ipFilter.getRemainingBanTime(host);
        foreach (Pinger* pinger, m_pingerList){
            if(pinger->hasHost(host))
                pinger->start(banTime);
        }
        Contact* contact = handshake->getContact();
        if(contact){
            m_pingerList.value(contact)->start(banTime);
        }
        delete handshake;
        m_handshakeList.remove(host);
    }

}

void SVoIP::restartListener(){
    m_listener.close();
    qint16 port = QSettings("settings.ini", QSettings::IniFormat).value("network/listen_port").toInt();
    m_listener.listen(QHostAddress::Any, port);
}

QPair<AppUID,AbstractApp*> SVoIP::startApp(Contact* contact, AppType appType){
    AbstractApp* app = NULL;
    AppUID localUID(appType);
    NetworkManager* netMgr = m_networkManagerList.value(contact, NULL);
    if(netMgr){
        if(m_appRegisterTable.contains(localUID) && m_appRegisterTable.value(localUID, NULL) == contact){
            app = m_appList.value(localUID);
            app->show();
        }else{
            foreach(AppUID existingAppUID, m_appList.keys())
                if(existingAppUID == localUID)
                    localUID.setInstanceID(localUID.instanceID() + 1);

            switch(appType){
                case Messenger:
                    app = new MessengerApp(contact);
                    break;
                case VoIP:
                    app = new VoiceApp(contact);
                    break;
                case VideoStreamer:
                    app = new VideoApp(contact);
                    break;
                default:
                    emit error(InvalidAppID);
                    break;
            }
            if(app){
                m_appList.insert(localUID, app);
                netMgr->registerApp(localUID, app);
                m_appRegisterTable.insert(localUID, contact);
                app->show();
            }
        }
    }
    return qMakePair(localUID, app);
}

void SVoIP::onStartAppRequest(Contact* contact, AppType appType){
    QPair<AppUID,AbstractApp*> appEntry = startApp(contact, appType);
    if(appEntry.second)
        m_networkManagerList.value(contact, NULL)->requestPartnerApp(appEntry.first);

}

void SVoIP::onStartAppForRequest(Contact* contact, AppUID distantUID){
    QPair<AppUID,AbstractApp*> appEntry = startApp(contact, distantUID.type());
    if(appEntry.second)
        m_networkManagerList.value(contact, NULL)->registerAppConnection(appEntry.first, distantUID);
}

void SVoIP::startHandshake(QTcpSocket* socket){
    Pinger* pinger = dynamic_cast<Pinger*>(sender());
    IpFilter* filter = dynamic_cast<IpFilter*>(sender());
    if(pinger || filter){
        QString host = socket->peerAddress().toString();
        if(getHostState(host) != NotConnected && host != "127.0.0.1")   //TODO: remove debug localhost
            socket->close();
        else{
            AbstractHandshake* handshake;
            if(pinger){
                handshake = new HandshakeStarter(pinger->takeSocket(), pinger->getContact(), &m_ipFilter, m_rsaKeyring, this);
            }else{
                foreach (Pinger* pinger, m_pingerList){
                    if(pinger->hasHost(host))
                        pinger->stop();
                }
                handshake = new HandshakeResponder(socket, m_contactDB, &m_ipFilter, m_rsaKeyring, this);
            }
            m_handshakeList.insertMulti(socket->peerAddress().toString(), handshake); //TODO: debug multi
            connect(handshake, &AbstractHandshake::success, this, &SVoIP::onHandshakeSuccess);
            connect(handshake, &AbstractHandshake::error, this, &SVoIP::onHandshakeError);
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
        //qDebug()<<"discovery error is : "+error;
    }

    if(!nat.add_port_mapping("svoip","10.207.3.65",port,port,"TCP")){
        error = nat.get_last_error();
        //qDebug()<<"add_port_mapping error is : "+error;
    }
    else {
        error = nat.get_last_error();
        //qDebug()<<"add port mapping succ.";
    }
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
SVoIP::IpState SVoIP::getHostState(QString host){
    foreach(NetworkManager* netMgr, m_networkManagerList) {
        if(netMgr->getHost() == host)
            return Connected;
    }
    if(m_handshakeList.contains(host)){
        return Handshaking;
    }else
        return NotConnected;
}
SVoIP::~SVoIP(){
    if(m_contactListWindow) delete m_contactListWindow;
    if(m_contactDB) delete m_contactDB;
    if(m_passwordWindow) delete m_passwordWindow;
    qDeleteAll(m_appList);
    qDeleteAll(m_networkManagerList);
}
