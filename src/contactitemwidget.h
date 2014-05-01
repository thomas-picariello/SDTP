#ifndef CONTACTITEMDELEGATE_H
#define CONTACTITEMDELEGATE_H

#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QMenu>

namespace Ui {
class ContactItemWidget;
}

class ContactItemWidget : public QWidget
{
    Q_OBJECT
public:
    enum Action{
        MessengerAction,
        CallAction,
        EditAction,
        DeleteAction
    };

    explicit ContactItemWidget(int contactId = 0,
                               QWidget *parent = 0);
    ~ContactItemWidget();

    int getContactId() const;
    void hide();
    void setContactId(int id);
    void show();

signals:
    void actionTriggered(int id, ContactItemWidget::Action action);

public slots:
    void onButtonEvent(QEvent *event);
    void onCallButtonClick();
    void onMessengerButtonClick();
    void onEditActionClick();
    void onDeleteActionClick();
    void showOptionsMenu();

private:
    Ui::ContactItemWidget *ui;
    int mContactId;
    QMenu *mOptionsMenu;
};

#endif // CONTACTITEMDELEGATE_H
