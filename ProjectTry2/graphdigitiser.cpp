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
    if(isCalibrating){
        switch(CalibrationCount){
        case 0:
            CalibrationX.push_back(event->pos());
            bool xmin;
            Xmin=QInputDialog::getDouble(this,tr("Xmin"),tr("Please enter the minimum value on X axis:"),0,-2147483647,2147483647,1,&xmin);
            CalibrationCount+=1;
            break;
        case 1:
            CalibrationX.push_back((event->pos()));
            CalibrationCount+=1;
            bool xmax;
            Xmax=QInputDialog::getDouble(this,tr("Xmax"),tr("Please enter the maximum value on X axis:"),0,-2147483647,2147483647,1,&xmax);
            break;
        case 2:
            CalibrationY.push_back(event->pos());
            CalibrationCount+=1;
            bool ymin;
            Ymin=QInputDialog::getDouble(this,tr("Ymin"),tr("Please enter the minimum value on Y axis:"),0,-2147483647,2147483647,1,&ymin);
            break;
        case 3:
            CalibrationY.push_back(event->pos());
            CalibrationCount+=1;
            bool ymax;
            Ymax=QInputDialog::getDouble(this,tr("Ymax"),tr("Please enter the maximum value on Y axis:"),0,-2147483647,2147483647,1,&ymax);
            CalibrateX();
            CalibrateY();
            isCalibrating=false;
            break;
        default:
            break;
        }
    }
    if(isSampling){
        Samples.push_back(event->pos());
    }
    if(isShowing){
        QVector<QPointF> show=CalculatePos(Samples,mode);
        qDebug("X: %f",show[show.size()-1].x());
        qDebug("Y: %f",show[show.size()-1].y());
        //ui->lcdNumber_x->display(show[show.size()-1].x());
        //ui->lcdNumber_y->display(show[show.size()-1].y());
    }
}

void GraphDigitiser::on_actionOpen_Image_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Open file"));
    QPixmap graph(file);
    scene->addPixmap(graph);
    sceneCheck=true;
}

void GraphDigitiser::on_actionCalibrate_triggered()
{
    isCalibrating=true;
    CalibrationX.clear();
    CalibrationY.clear();
    CalibrationCount=0;
}

void GraphDigitiser::CalibrateY()
{
    Calibration_factorY=(Ymax-Ymin)/(CalibrationY[1].y()-CalibrationY[0].y());
    qDebug("CalibrationY: %f", Calibration_factorY);
}

void GraphDigitiser::CalibrateX()
{
    Calibration_factorX=(Xmax-Xmin)/(CalibrationX[1].x()-CalibrationX[0].x());
    qDebug("CalibrationX: %f", Calibration_factorX);
}

QVector <QPointF> GraphDigitiser::CalculatePos(QVector<QPointF> &Vec,QString &mode)
{
    int modeNum=0;
    if(mode=="LinearLog"){
        modeNum=1;
    }else if(mode=="LinearLinear"){
        modeNum=2;
    }else if(mode=="LogLinear"){
        modeNum=3;
    }else if(mode=="LogLog"){
        modeNum=4;
    }
    qreal pointX=0,pointY=0;
    QVector<QPointF> CalibratedVec;
    switch(modeNum){
        case 1:
            for (int i=0;i<Vec.size();i++) {
                pointX=(Vec[i].x()-CalibrationX[0].x())*Calibration_factorX+Xmin;
                pointY=qPow(YLog,(Vec[i].y()-CalibrationY[0].y())*Calibration_factorY+Ymin);
                CalibratedVec.push_back(QPointF(pointX,pointY));
            }
            break;
        case 2:
            for (int i=0;i<Vec.size();i++) {
                pointX=(Vec[i].x()-CalibrationX[0].x())*Calibration_factorX+Xmin;
                pointY=(Vec[i].y()-CalibrationY[0].y())*Calibration_factorY+Ymin;
                CalibratedVec.push_back(QPointF(pointX,pointY));
            }
            break;
        case 3:
            for (int i=0;i<Vec.size();i++) {
                pointX=qPow(XLog,(Vec[i].x()-CalibrationX[0].x())*Calibration_factorX+Xmin);
                pointY=(Vec[i].y()-CalibrationY[0].y())*Calibration_factorY+Ymin;
                CalibratedVec.push_back(QPointF(pointX,pointY));
            }
            break;
        case 4:
            for (int i=0;i<Vec.size();i++) {
                pointX=qPow(XLog,(Vec[i].x()-CalibrationX[0].x())*Calibration_factorX+Xmin);
                pointY=qPow(YLog,(Vec[i].y()-CalibrationY[0].y())*Calibration_factorY+Ymin);
                CalibratedVec.push_back(QPointF(pointX,pointY));
            }
            break;
        default:
            break;
    }
    return CalibratedVec;
}

void GraphDigitiser::on_actionShow_data_triggered()
{
    isShowing=!isShowing;
}

void GraphDigitiser::on_actionManually_digitise_triggered()
{
    isSampling=true;
}

void GraphDigitiser::on_actionLinear_linear_triggered()
{
    mode="LinearLinear";
    XLog=1;
    YLog=1;
}

void GraphDigitiser::on_actionLinear_log_triggered()
{
    mode="LinearLog";
    XLog=1;
    YLog=1;
    bool ylog;
    YLog=QInputDialog::getDouble(this,tr("YLog"),tr("Please enter the base of Y axis:"),0,-2147483647,2147483647,1,&ylog);
}

void GraphDigitiser::on_actionlog_Linear_triggered()
{
    mode="LogLinear";
    XLog=1;
    YLog=1;
    bool xlog;
    XLog=QInputDialog::getDouble(this,tr("XLog"),tr("Please enter the base of X axis:"),0,-2147483647,2147483647,1,&xlog);
}

void GraphDigitiser::on_actionLog_Log_triggered()
{
    mode="LogLog";
    XLog=1;
    YLog=1;
    bool xlog;
    XLog=QInputDialog::getDouble(this,tr("XLog"),tr("Please enter the base of X axis:"),0,-2147483647,2147483647,1,&xlog);
    bool ylog;
    YLog=QInputDialog::getDouble(this,tr("YLog"),tr("Please enter the base of Y axis:"),0,-2147483647,2147483647,1,&ylog);
}

void GraphDigitiser::on_actionExport_Data_triggered()
{
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save as"),tr("*txt"));
    QFile file(fileName);
    QVector<QPointF> output = CalculatePos(Samples,mode);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    for (int i=0;i<output.size();i++) {
        out<<output[i].x()<<"    "<<output[i].y()<<"\n";
    }
    file.close();
}
