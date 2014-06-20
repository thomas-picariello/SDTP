#ifndef IPFILTER_H
#define IPFILTER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QHash>
#include <QDateTime>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>

class IpFilter : public QObject
{
    Q_OBJECT
public:
    explicit IpFilter(QObject *parent = 0);
    qint32 getRemainingBanTime(QString &ip) const;
    qint32 getBanTime(QString &ip) const;

signals:
    void accepted(QTcpSocket *socket);

public slots:
    void addBan(const QString &ip,
                const quint32 banDuration,
                const quint32 banTimestamp = QDateTime::currentDateTime().toTime_t());
    void filter(QTcpSocket *socket);

private slots:
    void clean();

private:
    QHash<QString,QPair<qint32,quint32>> m_TimedTable;
    QTimer m_cleanTimer;
    Q_DISABLE_COPY(IpFilter)
};

#endif // IPFILTER_H
