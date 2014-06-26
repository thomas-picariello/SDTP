#include "ipfilter.h"

IpFilter::IpFilter(QObject *parent) :
    QObject(parent)
{
}

void IpFilter::addBan(const QString &ip, const qint32 banDuration, const quint32 banTimestamp){
    m_TimedTable.insert(ip, qMakePair(banDuration, banTimestamp));
}

qint32 IpFilter::getLastBanTime(QString &ip) const{
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

void IpFilter::removeBan(const QString &ip){
    m_TimedTable.remove(ip);
}
