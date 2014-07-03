#include "appuid.h"

AppUID::AppUID():
    m_Type(Undefined),
    m_InstanceID(0)
{}

AppUID::AppUID(AppType typeId, quint16 instanceId):
    m_Type(typeId),
    m_InstanceID(instanceId)
{}

bool AppUID::operator<(const AppUID &second) const{
    if(m_Type == second.type())
        return (m_InstanceID < second.instanceID());
    return (m_Type < second.type());
}

bool AppUID::operator ==(const AppUID &second) const{
    return (m_Type == second.type()) && (m_InstanceID == second.instanceID());
}

QDataStream &operator<<(QDataStream &out, const AppUID& appUID){
    out << (quint8)appUID.type()
        << appUID.instanceID();
    return out;
}

QDataStream &operator>>(QDataStream &in, AppUID& appUID){
    quint8 _type;
    quint16 _instanceID;
    in >> _type;
    in >> _instanceID;
    appUID = AppUID(static_cast<AppType>(_type), _instanceID); //TODO: improve with moc (if possible)
    return in;
}
