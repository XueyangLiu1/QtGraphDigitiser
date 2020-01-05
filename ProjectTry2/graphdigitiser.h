#ifndef GRAPHDIGITISER_H
#define GRAPHDIGITISER_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QPixmap>
#include <QPointF>
#include <QMouseEvent>
#include <QInputDialog>
#include <QtMath>
#include <QDebug>
#include <QFileDialog>

namespace Ui {
class GraphDigitiser;
}

class GraphDigitiser : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphDigitiser(QWidget *parent = nullptr);
    ~GraphDigitiser();
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_actionOpen_Image_triggered();

    void on_actionCalibrate_triggered();

    void CalibrateX();

    void CalibrateY();

    QVector<QPointF> CalculatePos(QVector<QPointF> &Vec,QString &mode);

    void on_actionShow_data_triggered();

    void on_actionManually_digitise_triggered();

    void on_actionLinear_linear_triggered();

    void on_actionLinear_log_triggered();

    void on_actionlog_Linear_triggered();

    void on_actionLog_Log_triggered();

    void on_actionExport_Data_triggered();

private:
    Ui::GraphDigitiser *ui;
    QGraphicsScene *scene = new QGraphicsScene();
    bool sceneCheck=false;

    QVector<QPointF> CalibrationX;
    QVector<QPointF> CalibrationY;
    bool isCalibrating=false;
    int CalibrationCount=0;
    double Xmin=0,Xmax=0,Ymin=0,Ymax=0,Calibration_factorX=0,Calibration_factorY=0;

    QVector<QPointF> Samples;
    bool isSampling=false;

    bool isShowing;

    QString mode;
    double XLog=1,YLog=1;

};

#endif // GRAPHDIGITISER_H
