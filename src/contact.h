/**
 * @brief Contact represents a contact and its details.
 *
 * The contacts can be saved and erased using save() and erase().
 *
 * The ContactFactory should be used to retrieve existing contacts from the storage.
 *
 * Each contact is stored by the **QSettings** class according to the following path pattern:
 *
 * ~~~~~~~~~~
 * /"Contacts"/id/("name", "host", "port" or "key")
 * ~~~~~~~~~~
 */

#ifndef CONTACT_H
#define CONTACT_H

#include <QSettings>
#include <QtSql/QtSql>

class Contact: public QObject
{
    Q_OBJECT
public: 
    /**
     * @brief Constructs an empty contact object.
     *
     * The smallest available ID is used as default ID value.
     *
     * @param parent the parent **QObject**
     */
    explicit Contact(QObject *parent = 0);

    /**
     * @brief Constructs a contact object with all the fields defined.
     *
     * @param id the id of the contact
     * @param name the name of the contact
     * @param host the host name or the IP address of the contact
     * @param port the port to use for the contact
     * @param key the public key of the contact
     * @param parent the parent **QObject**
     */
    explicit Contact(int id,
            QString name,
            QString host,
            quint16 port,
            QByteArray key,
            QObject *parent = 0);

    /**
     * @brief Erases the contact from the storage.
     */
    void erase();

    /**
     * @brief Returns the hostname or the IP address of the contact.
     * @return the hostname or the IP address of the contact
     */
    QString getHost() const;

    /**
     * @brief Returns the unique ID of the contact.
     * @return the ID of the contact
     */
    int getId() const;

    /**
     * @brief Returns the unique public key of the contact.
     * @return the public key of the contact
     */
    QByteArray getKey() const;

    /**
     * @brief Returns the name of the contact.
     * @return the name of the contact
     */
    QString getName() const;

//    /**
//     * @brief Returns the next smallest available ID.
//     * @return the next available ID
//     */
//    static int getNextAvailableID();

    /**
     * @brief Returns the port to be used for the contact.
     * @return the port to use for the contact
     */
    quint16 getPort() const;

    /**
     * @brief Save the contact to the storage in its actual state.
     *
     * Some field might be left empty.
     */
    void save();

    /**
     * @brief Defines the unique ID of the contact.
     * @param id the unique id of the contact.
     */
    void setId(int id);

    /**
     * @brief Defines the name of the contact.
     * @param name the name of the contact.
     */
    void setName(QString name);

    /**
     * @brief Defines the hostname or the IP address of the contact.
     * @param host the hostname or the IP address of the contact.
     */
    void setHost(QString host);

    /**
     * @brief Defines the port to be used for the contact.
     * @param port the port to be used for the contact.
     */
    void setPort(quint16 port);

    /**
     * @brief Defines the unique public key of the contact.
     * @param key the public key of the contact.
     */
    void setKey(QByteArray key);

private :
    QString mHost;
    int mId;
    QByteArray mKey;
    quint16 mPort;
    QString mName;

    Q_DISABLE_COPY(Contact)
};

#endif // CONTACT_H
