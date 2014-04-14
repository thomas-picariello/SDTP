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
     * @brief Constructs an empty contact object.
     *
     * @param parent the parent **QObject**
     */
    explicit Contact(QObject *parent = 0);

    /**
     * @brief Constructs a contact object with all the fields defined.
     *
     * @param id the id of the contact
     * @param name the name of the contact
     * @param hostList the host list of the contact
     * @param port the port to use for the contact
     * @param key the public key of the contact
     * @param parent the parent **QObject**
     */
    explicit Contact(int id,
            QString name,
            QStringList hostsList,
            quint16 port,
            QByteArray key,
            QObject *parent = 0);

    /**
     * @brief Returns a copy of the hosts list of the contact.
     * @return a copy of the hosts list of the contact
     */
    QStringList getHostsList() const;

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

    /**
     * @brief Returns the port to be used for the contact.
     * @return the port to use for the contact
     */
    quint16 getPort() const;

    /**
     * @brief Returns a pointer to the internal hosts list of the contact.
     * @return a pointer to the internal hosts list of the contact
     */
    QStringList* hostsList();

    /**
     * @brief Defines the host list of the contact.
     * @param hostsList the host list to copy to the contact
     */
    void setHostsList(QStringList hostsList);

    /**
     * @brief Defines the unique ID of the contact.
     * @param id the unique id of the contact
     */
    void setId(int id);

    /**
     * @brief Defines the unique public key of the contact.
     * @param key the public key of the contact
     */
    void setKey(QByteArray key);

    /**
     * @brief Defines the name of the contact.
     * @param name the name of the contact
     */
    void setName(QString name);

    /**
     * @brief Defines the port to be used for the contact.
     * @param port the port to be used for the contact
     */
    void setPort(quint16 port);

private :
    QStringList mHostsList;
    int mId;
    QByteArray mKey;
    quint16 mPort;
    QString mName;

    Q_DISABLE_COPY(Contact)
};

#endif // CONTACT_H
