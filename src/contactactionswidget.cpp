#include "contactactionswidget.h"
#include "ui_contactactionswidget.h"

ContactActionsWidget::ContactActionsWidget(int contactId, QWidget *parent) :
    QWidget(parent), ui(new Ui::ContactActionsWidget)
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

int ContactActionsWidget::getContactId() const{
    return mContactId;
}

void ContactActionsWidget::hide(){
    ui->call_bt->hide();
    ui->messenger_bt->hide();
    ui->options_bt->hide();
}

void ContactActionsWidget::onButtonEvent(QEvent *event){
    QPushButton* button = dynamic_cast<QPushButton*>(sender());
    qDebug()<<event->type();
    switch(event->type()){
    case QEvent::MouseMove:
        break;
    }
}

void ContactActionsWidget::onCallButtonClick(){
    emit(actionTriggered(mContactId, CallAction));
}

void ContactActionsWidget::onMessengerButtonClick(){
    emit actionTriggered(mContactId, MessengerAction);
}

void ContactActionsWidget::onEditActionClick(){
    emit actionTriggered(mContactId, EditAction);
}

void ContactActionsWidget::onDeleteActionClick(){
    emit actionTriggered(mContactId, DeleteAction);
}


void ContactActionsWidget::setContactId(int id){
    mContactId = id;
}

void ContactActionsWidget::show(){
    ui->call_bt->show();
    ui->messenger_bt->show();
    ui->options_bt->show();
}

void ContactActionsWidget::showOptionsMenu(){
    QPoint pos = ui->options_bt->geometry().bottomLeft();
    pos = ui->options_bt->mapFromParent(pos);
    pos = ui->options_bt->mapToGlobal(pos);
    mOptionsMenu->exec(pos);
}

ContactActionsWidget::~ContactActionsWidget(){
    delete ui;
}
