/**
 * @brief Contact represents a contact and its details.
 */

#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QStringList>

class Contact: public QObject
{
    Q_OBJECT
public: 
     /**
     * @brief The Status enum describs the possibles states for a Contact.
     */
    enum Status{
        Offline,
        Online,
        Busy,
        Away
    };

    /**
     * @brief Constructs an empty Contact object.
     *
     * @param parent the parent **QObject**
     */
    explicit Contact(QObject *parent = 0);

    /**
     * @brief Constructs a Contact object with all the fields defined.
     *
     * @param id the id of the Contact
     * @param name the name of the Contact
     * @param hostList the host list of the Contact
     * @param port the port to use for the Contact
     * @param key the public key of the Contact
     * @param parent the parent **QObject**
     */
    explicit Contact(int id,
            QString name,
            QStringList hostsList,
            quint16 port,
            QByteArray key,
            QObject *parent = 0);

    /**
     * @brief activeHost returns the currently online host.
     * @return the host, or an empty string if none has been defined
     */
    QString getActiveHost() const;

    /**
     * @brief Returns a copy of the hosts list of the Contact.
     * @return a copy of the hosts list of the Contact
     */
    QStringList getHostsList() const;

    /**
     * @brief Returns the unique ID of the Contact.
     * @return the ID of the Contact
     */
    uint getId() const;

    /**
     * @brief Returns the unique public key of the Contact.
     * @return the public key of the Contact
     */
    QByteArray getKey() const;

    /**
     * @brief Returns the name of the Contact.
     * @return the name of the Contact
     */
    QString getName() const;

    /**
     * @brief Returns the port to be used for the Contact.
     * @return the port to use for the Contact
     */
    quint16 getPort() const;

    /**
     * @brief Returns the Status of the Contact.
     * @return the Status of the Contact
     */
    Status getStatus() const;

    /**
     * @brief Returns a pointer to the internal hosts list of the Contact.
     * @return a pointer to the internal hosts list of the Contact
     */
    QStringList* hostsList();

    /**
     * @brief Defines the active host from the host list
     * @param activeHost the active host.
     */
    void setActiveHost(QString& activeHost);

    /**
     * @brief Defines the host list of the Contact.
     * @param hostsList the host list to copy to the Contact
     */
    void setHostsList(QStringList hostsList);

    /**
     * @brief Defines the unique ID of the Contact.
     * @param id the unique id of the Contact
     */
    void setId(uint id);

    /**
     * @brief Defines the unique public key of the Contact.
     * @param key the public key of the Contact
     */
    void setKey(QByteArray key);

    /**
     * @brief Defines the name of the Contact.
     * @param name the name of the Contact
     */
    void setName(QString name);

    /**
     * @brief Defines the port to be used for the Contact.
     * @param port the port to be used for the Contact
     */
    void setPort(quint16 port);

    /**
     * @brief Defines the Status of the Contact.
     * @param status the Status to be set
     */
    void setStatus(Status status);

    /**
     * @brief operator == return true if both Contact have the same id, false otherwise.
     * @param other the other Contact to be compared with
     * @return true if the ids are the same
     */
    bool operator ==(const Contact &other) const;

    /**
     * @brief operator < return true if the other Contact has a higher id, false otherwise.
     * @param other the other Contact to be compared with
     * @return true if the other's id is higher
     */
    bool operator <(const Contact &other) const;

signals:
    /**
     * @brief signal emited when setStatus is called.
     */
    void statusChanged();

private :
    QStringList m_hostsList;
    QString m_activeHost;
    uint m_id;
    QByteArray m_key;
    QString m_name;
    quint16 m_port;
    Status m_status;

    Q_DISABLE_COPY(Contact)
};

#endif // CONTACT_H
