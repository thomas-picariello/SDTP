#include "ipfilter.h"

IpFilter::IpFilter(QObject *parent) :
    QObject(parent)
{
    connect(&m_cleanTimer, SIGNAL(timeout()),
            this, SLOT(clean()));
    m_cleanTimer.start(10000); //clean list every 10s
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
        int remainingTime = (timePair.second + timePair.first) - currentTime;
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
    foreach(QString ip, m_TimedTable.keys()){
        QPair<qint32,quint32> timePair = m_TimedTable.value(ip);
        int diffTime = (timePair.second + timePair.first) - currentTime;
        if(diffTime <= 0)
            m_TimedTable.remove(ip);
    }
}
