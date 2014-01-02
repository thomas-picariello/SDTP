#ifndef MESSERGERWINDOW_H
#define MESSERGERWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QByteArray>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QStringList>
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"

namespace Ui {
class MessengerWindow;
}

class MessengerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MessengerWindow(QWidget *parent = 0);
    ~MessengerWindow();

signals:
    void sendMessage(QByteArray data);

public slots:
    void displayMessage(QString);
    void onSend();

private:
    Ui::MessengerWindow *ui;
    QStringList *msglist;
};

#endif // MESSERGERWINDOW_H
