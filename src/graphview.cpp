#include "graphview.h"

GraphView::GraphView(QWidget *parent) : QWidget(parent){
    mImg = new QImage(512, 512, QImage::Format_Mono);

    mImgLabel = new QLabel(this);
    mImgLabel->resize(512, 512);

    resize(512,512);
    setWindowTitle("VisualRandomTester");

    mSoundSampler = new SoundSampler();
    mSoundSampler->getSample();

    QPainter painter;
    painter.begin(mImg);
    painter.fillRect(rect(),QBrush(Qt::black));
    painter.end();

    connect(mSoundSampler, SIGNAL(sampleAvailable(QByteArray)),
            this, SLOT(sampleHandler(QByteArray)));

    qsrand(QTime::currentTime().second());
}

void GraphView::paintEvent(QPaintEvent *){
    mImgLabel->setPixmap(QPixmap::fromImage(*mImg));
}

void GraphView::sampleHandler(QByteArray sample){

    QPainter painter;
    QPoint point(qrand()%512, qrand()%512);

    painter.begin(mImg);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawPoint(point);
    painter.end();

    update();
    mSoundSampler->getSample();
}


GraphView::~GraphView(){
    delete mSoundSampler, mTimer, mImg;
}
