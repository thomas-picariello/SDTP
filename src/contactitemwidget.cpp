#include "contactitemwidget.h"
#include "ui_contactitemwidget.h"

ContactItemWidget::ContactItemWidget(int contactId, QWidget *parent) :
    QWidget(parent), ui(new Ui::ContactItemWidget)
{
    mContactId = contactId;
    ui->setupUi(this);

    mOptionsMenu = new QMenu(this);
    mOptionsMenu->addAction(tr("Edit"),
                            this, SLOT(onEditActionClick()));
    mOptionsMenu->addAction(tr("Delete"),
                            this, SLOT(onDeleteActionClick()));

    connect(ui->messenger_bt, SIGNAL(clicked()),
            this, SLOT(onMessengerButtonClick()));
    connect(ui->call_bt, SIGNAL(clicked()),
            this, SLOT(onCallButtonClick()));
    connect(ui->options_bt, SIGNAL(clicked()),
            this, SLOT(showOptionsMenu()));
}

int ContactItemWidget::getContactId() const{
    return mContactId;
}

void ContactItemWidget::hide(){
    ui->call_bt->hide();
    ui->messenger_bt->hide();
    ui->options_bt->hide();
}

void ContactItemWidget::onButtonEvent(QEvent *event){
    QPushButton* button = dynamic_cast<QPushButton*>(sender());
    qDebug()<<event->type();
    switch(event->type()){
    case QEvent::MouseMove:
        break;
    }
}

void ContactItemWidget::onCallButtonClick(){
    emit(actionTriggered(mContactId, CallAction));
}

void ContactItemWidget::onMessengerButtonClick(){
    emit actionTriggered(mContactId, MessengerAction);
}

void ContactItemWidget::onEditActionClick(){
    emit actionTriggered(mContactId, EditAction);
}

void ContactItemWidget::onDeleteActionClick(){
    emit actionTriggered(mContactId, DeleteAction);
}


void ContactItemWidget::setContactId(int id){
    mContactId = id;
}

void ContactItemWidget::show(){
    ui->call_bt->show();
    ui->messenger_bt->show();
    ui->options_bt->show();
}

void ContactItemWidget::showOptionsMenu(){
    QPoint pos = ui->options_bt->geometry().bottomLeft();
    pos = ui->options_bt->mapFromParent(pos);
    pos = ui->options_bt->mapToGlobal(pos);
    mOptionsMenu->exec(pos);
}

ContactItemWidget::~ContactItemWidget(){
    delete ui;
}
