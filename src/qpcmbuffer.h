/**
 * @brief QPcmBuffer provides a QIODevice interface to a QVector<qint16>
 * reprensenting an array of PCM samples.
 *
 * The samples are signed integers with a size of 16 bits (qint16).
 *
 * QPcmBuffer is similar to **QBuffer**
 * except that the internal buffer is a **QVector<qint16>**
 * in order to be stored contiguously and accessed as a sample array.
 */

#ifndef QPCMBUFFER_H
#define QPCMBUFFER_H

#include <QIODevice>
#include <QVector>
#include <QtEndian>

class QPcmBuffer : public QIODevice
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an empty buffer with the given *parent*.
     *
     * It is initialized with an internal QVector<qint16> and the maximum size is set to 0 (no limit).
     *
     * You can call setData() to fill the buffer with data,
     * or you can open it in write mode and use write().
     *
     * @param parent the parent **QObject**
     */
    explicit QPcmBuffer(QObject* parent = 0);

    /**
     * @brief Constructs an empty buffer with the *maximum size* for the buffer and the given *parent*.
     *
     * It is initialized with an internal **QVector<qint16>**.
     *
     * @param maxSize the maximum size of the buffer
     * @param parent the parent **QObject**
     */
    explicit QPcmBuffer(qint64 maxSize, QObject* parent = 0);

    /**
     * @brief Constructs a QPcmBuffer that uses the **QVector<qint16>** pointer by *buffer*.
     *
     * The caller is responsible for ensuring that *buffer* remains valid until the QPcmBuffer is destroyed,
     * or until setBuffer() is called to change the buffer.
     * QPcmBuffer doesn't take ownership of the **QVector**.
     *
     * @param buffer a pointer to a **QVector<qint16>** to be used as buffer
     * @param parent the parent **QObject**
     */
    explicit QPcmBuffer(QVector<qint16> *buffer, QObject* parent = 0);

    /**
     * @brief Destroys the QPcmBuffer.
     */
    ~QPcmBuffer();

    /**
     * @brief Returns a reference to the internal buffer
     * @return A reference to the internal buffer as a **QVector<qint16>**
     */
    QVector<qint16>& buffer();

    /**
     * @brief Returns a pointer to the internal buffer array.
     *
     * @return A pointer to the internal **QVector<qint16>** data
     */
    qint16* data();

    /**
     * @brief This is an overloaded function.
     *
     * See data().
     */
    const qint16* data() const;

    /**
     * @brief Return the buffer maximum size (in number of samples).
     *
     * Returns *0* when the buffer has an unlimited size.
     *
     * @return The maximum size of the internal buffer
     */
    qint64 getMaxSize() const;

    /**
     * @brief Pre allocate space at the end of the buffer to write in it using the returned pointer.
     *
     * If there is no more space in the buffer to allocate at the end (due to defined buffer max size),
     * the vector will be shifted, clearing the oldest data.
     * Be aware that the allocated space cannot be larger than
     * the defined maximum buffer size (if set).
     *
     * **Warning:** A pointer previously returned by buffer() or data()
     * might **not be valid** after this operation !
     *
     * @param sampleCount the number to pre allocate
     * @return A pointer to the begining of the allocated space
     */
    qint16* preAllocate(qint64 sampleCount);

    /**
     * @brief Makes QPcmBuffer uses the **QVector<qint16>** pointed to by *buffer* as its internal buffer.
     *
     * The caller is responsible for ensuring that *buffer* remains valid until the QPcmBuffer is destroyed,
     * or until setBuffer() is called to change the buffer.
     * QPcmBuffer doesn't take ownership of the QVector.
     *
     * @param buffer a pointer to the **QVector<qint16>** to be used
     */
    void setBuffer(QVector<qint16> *buffer);

    /**
     * @brief Sets the contents of the internal buffer to be data.
     *
     * This is the same as assigning data to buffer().
     *
     * @param data a reference to the data to copy
     */
    void setData(const QVector<qint16> &data);

    /**
     * @brief Defines the internal buffer max allocatable space.
     *
     * If set to *0* the buffer has an unlimited size.
     *
     * @param maxSize the maximum sample count to be contained in the buffer
     */
    void setMaxSize(qint64 maxSize);

    /**
     * @brief Returns the actual size of the internal buffer.
     *
     * The size is the number of samples present in the buffer.
     *
     * @return The actual size of the internal buffer
     */
    qint64 size() const;

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 size);

private:
    QVector<qint16> *mBuffer;
    qint64 mBufferMaxSize;
    bool hasExternalBuffer;

    void free(qint64 size);

    Q_DISABLE_COPY(QPcmBuffer)
};

#endif // QPCMBUFFER_H
