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
#include <QMenu>
#include <QPoint>
#include <QColor>
#include <QMessageBox>
#include <QRect>

namespace Ui {
class GraphDigitiser;
}

class GraphDigitiser : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphDigitiser(QWidget *parent = nullptr);
    ~GraphDigitiser();
    void mousePressEvent(QMouseEvent *event);//detect mousepressevent and react
    void contextMenuEvent(QContextMenuEvent *event);//detect the action of right click and show the context menu
    void ConstructAllMenus(); //construct all the menus in the programme
    void auto_find_next(QPoint &point,int &ForwardOrBack);
    //a recursive function to detect points in the automatic mode, detailed explanation in the definition and report
    void toggleSceneActions(bool &sceneCheck);
    //call it when scene is added or changed to set some actions enabled or disabled
    void toggleAfterCalibrationActions(bool &CalibrationFinished);
    //call it when calibration is finished to set some actions enabled or disabled

private slots:
    void on_actionOpen_Image_triggered();//Load image to and set it as the scene

    void on_actionCalibrate_triggered();//To calibrate

    void CalibrateX();//calculate x factor

    void CalibrateY();//calculate y factor

    QVector<QPointF> CalculatePos(QVector<QPointF> &Vec,QString &mode);
    //used to convert pixel coordinates to real coordinates, mode refers to 4 kinds:linearlog,loglinear,linearlinear and loglog

    void on_actionManually_digitise_triggered();//enter the manually digitise process

    void on_actionLinear_linear_triggered();//set linearlinear mode

    void on_actionLinear_log_triggered();//set linearlog mode

    void on_actionlog_Linear_triggered();//set loglinear mode

    void on_actionLog_Log_triggered();//set loglog mode

    void on_actionExport_Data_triggered();//export data to file(default csv file)

    void on_actionAjust_Scale_triggered();//to adjust the scale

    void on_actionAuto_digitise_triggered();//enter the automatically digitise process

    void on_actionInformation_triggered();//pop up an information messagebox containing information about the program

private:
    Ui::GraphDigitiser *ui;
    QGraphicsScene *scene = new QGraphicsScene();//create a new scene
    bool sceneCheck=false;//this bool shows whether a image is set as the scene

    QVector<QPointF> CalibrationX;//used to store the pixel coordinates of 2 points, which are used to calculate the X factor
    QVector<QPointF> CalibrationY;//used to store the pixel coordinates of 2 points, which are used to calculate the Y factor
    bool isCalibrating=false;//used to indicate the calibration is in process
    bool calibrationFinished=false;//used to show that the calibration process is finished
    int CalibrationCount=0;
    //because there are 4 steps in the calibration process, this is used later in the mouse event with a switch case ,where the 4 cases runs seperately in 4 mouse left clicks
    //explained in details in the cpp file and report
    double Xmin=0,Xmax=0,Ymin=0,Ymax=0,Calibration_factorX=0,Calibration_factorY=0;//some variables and their initialization

    QVector<QPointF> manualSamples;//used to store QPointF of manually sampled points
    bool isManual=false;//used to indicate the manually sampling is in process
    QVector<QPoint> autoSamples;//used to store QPointF of automatically sampled points
    bool isAuto=false;//used to indicate the auto-sampling is in process
    int leftStep=-2;//default auto-sampling interval is 2 pixels
    int rightStep=2;//default auto-sampling interval is 2 pixels

    QString mode;//mode refers to 4 kinds:linearlog,loglinear,linearlinear and loglog
    double XLog=1,YLog=1;//the base of XLog and YLog is 1

    QMenu *p_pri_menu=new QMenu(this); //initialize the context menu
    QMenu *p_sec_menu=new QMenu(p_pri_menu);//initialize the secondary menu of context menu
};

#endif // GRAPHDIGITISER_H
