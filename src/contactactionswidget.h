#ifndef CONTACTACTIONSWIDGET_H
#define CONTACTACTIONSWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QMenu>

namespace Ui {
class ContactActionsWidget;
}

class ContactActionsWidget : public QWidget
{
    Q_OBJECT
public:
    enum Action{
        MessengerAction,
        CallAction,
        EditAction,
        DeleteAction
    };

    explicit ContactActionsWidget(int contactId = 0,
                               QWidget *parent = 0);
    ~ContactActionsWidget();

    int getContactId() const;
    void hide();
    void setContactId(int id);
    void show();

signals:
    void actionTriggered(int id, ContactActionsWidget::Action action);

public slots:
    void onButtonEvent(QEvent *event);
    void onCallButtonClick();
    void onMessengerButtonClick();
    void onEditActionClick();
    void onDeleteActionClick();
    void showOptionsMenu();

private:
    Ui::ContactActionsWidget *ui;
    int mContactId;
    QMenu *mOptionsMenu;
};

#endif // CONTACTACTIONSWIDGET_H
