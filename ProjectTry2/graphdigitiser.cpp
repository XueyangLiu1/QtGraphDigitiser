#include "graphdigitiser.h"
#include "ui_graphdigitiser.h"

GraphDigitiser::GraphDigitiser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphDigitiser)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    setCentralWidget(ui->graphicsView);
}

GraphDigitiser::~GraphDigitiser()
{
    delete ui;
}

void GraphDigitiser::mousePressEvent(QMouseEvent *event)
{
    if(isCalibratingX){
        CalibrationX.push_back(event->pos());
    }
    if(isCalibratingY){
        CalibrationY.push_back(event->pos());
    }
    if(isSampling){
        Samples.push_back(event->pos());
    }
}

void GraphDigitiser::on_actionOpen_Image_triggered(bool checked)
{
    if(checked){

    }
    else {
        checked=true;
        QString file = QFileDialog::getOpenFileName(this,tr("Open file"));
        QPixmap graph(file);
        scene->addPixmap(graph);
    }
}
