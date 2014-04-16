#include "contactitemwidget.h"
#include "ui_contactitemwidget.h"

ContactItemWidget::ContactItemWidget(int id, QWidget *parent) :
    QWidget(parent), ui(new Ui::ContactItemWidget)
{
    mId = id;
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

int ContactItemWidget::getId() const{
    return mId;
}

void ContactItemWidget::hide(){
    QWidget::hide();
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
    emit(actionTriggered(mId, CallAction));
}

void ContactItemWidget::onMessengerButtonClick(){
    emit actionTriggered(mId, MessengerAction);
}

void ContactItemWidget::onEditActionClick(){
    emit actionTriggered(mId, EditAction);
}

void ContactItemWidget::onDeleteActionClick(){
    emit actionTriggered(mId, DeleteAction);
}


void ContactItemWidget::setId(int id){
    mId = id;
}

void ContactItemWidget::show(){
    QWidget::show();
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
