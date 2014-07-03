#ifndef APPUID_H
#define APPUID_H

#include <QDataStream>

#include "typesenums.h"

class AppUID
{
public:
    AppUID();
    AppUID(AppType typeId, quint16 instanceId = 0);

    inline AppType type() const{ return m_Type; }
    inline quint16 instanceID() const{ return m_InstanceID; }
    inline void setType(AppType type){ m_Type = type; }
    inline void setInstanceID(quint16 id){ m_InstanceID = id; }

    bool operator<(const AppUID &second) const;
    bool operator==(const AppUID &second) const;

private:
    AppType m_Type;
    quint16 m_InstanceID;
};

QDataStream& operator<<(QDataStream &out, const AppUID& appUID);
QDataStream& operator>>(QDataStream &in, AppUID& appUID);

#endif // APPUID_H
