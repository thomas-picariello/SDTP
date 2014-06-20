#include "ipfilter.h"

IpFilter::IpFilter(QObject *parent) :
    QObject(parent)
{
    connect(&m_cleanTimer, SIGNAL(timeout()),
            this, SLOT(clean()));
    m_cleanTimer.start(60000); //clean list every minute
}

void IpFilter::addBan(const QString &ip, const quint32 banDuration, const quint32 banTimestamp){
    m_TimedTable.insert(ip, qMakePair(banDuration, banTimestamp));
}

qint32 IpFilter::getBanTime(QString &ip) const{
    return m_TimedTable.value(ip).first;
}

qint32 IpFilter::getRemainingBanTime(QString &ip) const{
    if(m_TimedTable.contains(ip)){
        QPair<qint32,quint32> timePair = m_TimedTable.value(ip);
        uint currentTime = QDateTime::currentDateTime().toTime_t();
        int remainingTime = currentTime - (timePair.second + timePair.first);
        if(remainingTime > 0)
            return remainingTime;
    }
    return 0;
}

void IpFilter::filter(QTcpSocket *socket){
    if(getRemainingBanTime(socket->peerAddress().toString()) == 0)
        emit accepted(socket);
    else
        delete socket;
}

void IpFilter::clean(){
    uint currentTime = QDateTime::currentDateTime().toTime_t();
    QHash<QString,QPair<qint32,quint32>>::iterator i = m_TimedTable.begin();
    while (i != m_TimedTable.end()) {
        if((currentTime - (i.value().second + i.value().first)) <= 0){
            m_TimedTable.remove(i.key());
        }
        ++i;
    }
}
