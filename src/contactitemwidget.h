#ifndef CONTACTITEMDELEGATE_H
#define CONTACTITEMDELEGATE_H

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

    explicit ContactItemWidget(int id = 0,
                               QWidget *parent = 0);
    ~ContactItemWidget();

    int getId() const;
    void hide();
    void setId(int id);
    void show();

signals:
    void actionTriggered(int id, ContactItemWidget::Action action);

private slots:
    void onCallButtonClick();
    void onMessengerButtonClick();
    void onEditActionClick();
    void onDeleteActionClick();
    void showOptionsMenu();

private:
    Ui::ContactItemWidget *ui;
    int mId;
    QMenu *mOptionsMenu;
};

#endif // CONTACTITEMDELEGATE_H
