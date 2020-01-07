#include "graphdigitiser.h"
#include "ui_graphdigitiser.h"

GraphDigitiser::GraphDigitiser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphDigitiser)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(scene);//set scene as the scene

    setCentralWidget(ui->graphicsView);//set central

    ConstructAllMenus();//construct all menus
    ui->actionExport_Data->setDisabled(true);
    toggleSceneActions(sceneCheck);//set the disabled status of all actions which should only be used after an image is set as the scene
    toggleAfterCalibrationActions(calibrationFinished);//set the disabled status of all actions which should only be used after the calibration is finished
}

GraphDigitiser::~GraphDigitiser()
{
    delete ui;
    delete scene;
    delete p_sec_menu;
    delete p_pri_menu;
}

void GraphDigitiser::mousePressEvent(QMouseEvent *event)
{
        if(isCalibrating){//when the calibration is in process
            switch(CalibrationCount){
            case 0:
                //at the fist click, the minimum position of x axis should be clicked and the minimum value of x axis should be entered
                CalibrationX.push_back(event->pos());//store the Xmin point at index:0 of CalibrationX
                bool xmin;
                Xmin=QInputDialog::getDouble(this,tr("Xmin"),tr("Please enter the minimum value on X axis:"),0,-2147483647,2147483647,3,&xmin);
                CalibrationCount+=1;
                //the count adds 1 so that at next click, the second step of calibration can process(case 1)
                break;
            case 1:
                //at the second click, the maximum position of x axis should be clicked and the maximum value of x axis should be entered
                CalibrationX.push_back((event->pos()));//store the Xmax point at index:1 of CalibrationX
                bool xmax;
                Xmax=QInputDialog::getDouble(this,tr("Xmax"),tr("Please enter the maximum value on X axis:"),0,-2147483647,2147483647,3,&xmax);
                CalibrationCount+=1;
                //the count adds 1 so that at next click, the third step of calibration can process(case 2)
                break;
            case 2:
                //at the third click, the minimum position of y axis should be clicked and the minimum value of y axis should be entered
                CalibrationY.push_back(event->pos());
                //store the Ymin point at index:0 of CalibrationY
                bool ymin;
                Ymin=QInputDialog::getDouble(this,tr("Ymin"),tr("Please enter the minimum value on Y axis:"),0,-2147483647,2147483647,3,&ymin);
                CalibrationCount+=1;
                //the count adds 1 so that at next click, the forth step of calibration can process(case 3)
                break;
            case 3:
                //at the forth click, the maximum position of y axis should be clicked and the maximum value of y axis should be entered
                CalibrationY.push_back(event->pos());
                //store the Ymax point at index:1 of CalibrationY
                bool ymax;
                Ymax=QInputDialog::getDouble(this,tr("Ymax"),tr("Please enter the maximum value on Y axis:"),0,-2147483647,2147483647,3,&ymax);
                CalibrateX();//calculate x factor
                CalibrateY();//calculate y factor
                isCalibrating=false;
                calibrationFinished=true;
                //the calibration process is finished
                toggleAfterCalibrationActions(calibrationFinished);
                //set the enabled status of all actions which should only be used after the calibration is finished
                break;
            default:
                break;
            }
        }
        if(isManual){
            if(CalibrationX.size()==2&&CalibrationY.size()==2){//examine if the calibration process is finished
                QPoint get_p=event->pos();
                QPixmap pixmap=this->grab(QRect(get_p,QSize(1,1)));
                QColor color=pixmap.toImage().pixel(0,0);
                if(!(color.red()==255&&color.blue()==255&&color.green()==255)){
                    //the above 4 lines grab a pixel's color and judge if it is white or not
                    manualSamples.push_back(event->pos());
                    //the pixel sample can only be in colors other than white
                    ui->actionExport_Data->setEnabled(manualSamples.size()>0);
                    //set export data action as enabled when at least one sample is generated
                }else{
                    QMessageBox::information(this,tr("Warning!"),tr("You have to choose a point on the curve!"));
                    //if the selected point is white, a message is poped
                }
            }else{
                QMessageBox::information(this,tr("Warning!"),tr("You have to calibrate first!"));
                //if the calibration is not finished
            }
        }
        if(isAuto){
            QPoint get_p=event->pos();
            QPixmap pixmap=this->grab(QRect(get_p,QSize(1,1)));
            QColor color=pixmap.toImage().pixel(0,0);
            if(!(color.red()==255&&color.blue()==255&&color.green()==255)){
                //the above 4 lines grab a pixel's color and judge if it is white or not
                isAuto=false;
                //the pixel sample can only be in colors other than white
                //as long as one point on the curve is chosen, the auto process will be conducted below
                //isAuto is set to false becasue next mouse click should not go in this if(isAuto){}
                if(CalibrationX.size()==2&&CalibrationY.size()==2){
                    //examine if the calibration process is finished
                    auto_find_next(get_p,leftStep);
                    auto_find_next(get_p,rightStep);
                    //two functions are called, one samples to the positive direction of x axis, the other samples to the negative
                    ui->actionExport_Data->setEnabled(autoSamples.size()>0);
                    //set export data action as enabled when at least one sample is generated
                }else{
                    QMessageBox::information(this,tr("Warning!"),tr("You have to calibrate first!"));
                    //if the calibration is not finished
                }
            }else{
                QMessageBox::information(this,tr("Warning!"),tr("You have to choose a point on the curve!"));
                //if the selected point is white, a message is poped
            }
        }
}

void GraphDigitiser::contextMenuEvent(QContextMenuEvent *event)
{
    p_pri_menu->popup(event->globalPos());
    //the primary menu pops up when the user right click
}


void GraphDigitiser::ConstructAllMenus()
{
    // This is a normal function that is executed whenever its name is called

    //  Creating the primary menu:
    p_pri_menu->addAction(ui->actionOpen_Image); // Add action to the primary window
    p_pri_menu->addSeparator(); // Line sperating entries in the menu
    p_pri_menu->addAction(ui->actionExport_Data); // Add action to the primary window
    p_pri_menu->addSeparator(); // Line sperating entries in the menu
    p_pri_menu->addAction(ui->actionCalibrate); // Add action to the primary window
    p_pri_menu->addSeparator(); // Line sperating entries in the menu
    p_pri_menu->addAction(ui->actionManually_digitise); // Add action to the primary window
    p_pri_menu->addSeparator(); // Line sperating entries in the menu
    p_pri_menu->addAction(ui->actionAuto_digitise); // Add action to the primary window
    p_pri_menu->addSeparator(); // Line sperating entries in the menu
    p_pri_menu->addMenu(p_sec_menu); // Add the scondary menu as a child to the primary menu

    // Creates the secondary menu (we want it to be diplayed as a child menu of the primary menu
    p_sec_menu->setTitle("Mode"); // Title of the "Secondary menu"
    p_sec_menu->addAction(ui->actionLinear_linear); // Add an action to the secondary menu
    p_sec_menu->addSeparator(); // Line sperating entries in the menu
    p_sec_menu->addAction(ui->actionLinear_log); // Add action to the secondary window
    p_sec_menu->addSeparator(); // Line sperating entries in the menu
    p_sec_menu->addAction(ui->actionlog_Linear); // Add action to the secondary window
    p_sec_menu->addSeparator(); // Line sperating entries in the menu
    p_sec_menu->addAction(ui->actionLog_Log); // Add action to the secondary window

}

void GraphDigitiser::auto_find_next(QPoint &point, int &ForwardOrBack)
{
    //this is a recursive function, which will keep looking for a next valid point when one is already found
    //the Qpoint &point is the already found one, and int &ForwardOrBack is the seeking direction for the next one
    //this is set default as 2 and -2,which means try to find next point every 2 pixels along the x axis, in both direction
    int next_x=point.x()+ForwardOrBack;
    //next_x indicates the x coordinate of the next possible valid point
    if(next_x>=CalibrationX[0].x()&&next_x<=CalibrationX[1].x()){
        //make sure the x coordinate of the next point is in the constraint
        QVector<QPoint> possible_points;
        //a new QVector to store possible points
        int y_upper=int(CalibrationY[1].y())+1;
        int y_lower=int(CalibrationY[0].y())-1;
        //this is the y constraint of all points at the next point's x coordinate
        for (int i=y_upper;i<y_lower;i++) {
            //in this constraint examine all points
            QPoint *potential_next=new QPoint(next_x,i);
            QPixmap pixmap=this->grab(QRect(*potential_next,QSize(1,1)));
            QColor color=pixmap.toImage().pixel(0,0);
            if(!(color.red()==255&&color.blue()==255&&color.green()==255)){
                possible_points.push_back(*potential_next);
                //if the point is not white, push it into the possible points vector for later use
            }
        }
        if(possible_points.size()>0){
            //if any point with a color other than white exists
            if(possible_points.size()%2==0){
                //if the number of possible points is even
                if(ForwardOrBack<0){
                    autoSamples.push_front(possible_points[possible_points.size()/2-1]);
                    //push the one with index:size/2-1 into autosamples as a valid sample
                    //if the direction is negative along x axis,use pushfront
                }else{
                    autoSamples.push_back(possible_points[possible_points.size()/2-1]);
                    //push the one with index:size/2-1 into autosamples as a valid sample
                    //if positive, use push back
                    //to make sure points in the sample is sorted ascendingly according to the value of x
                }
                auto_find_next(possible_points[possible_points.size()/2-1],ForwardOrBack);
                //recursively, according to the sample just generated, find the next possible one along the same direction
            }else if (possible_points.size()%2==1) {
                //if the number of possible points is odd
                if(ForwardOrBack<0){
                    autoSamples.push_front(possible_points[(possible_points.size()-1)/2]);
                    //push the one with index:(size-1)/2 into autosamples as a valid sample
                    //if the direction is negative along x axis,use pushfront
                }else{
                    autoSamples.push_back(possible_points[(possible_points.size()-1)/2]);
                    //push the one with index:(size-1)/2 into autosamples as a valid sample
                    //if positive, use push back
                    //to make sure points in the sample is sorted ascendingly according to the value of x
                }
                auto_find_next(possible_points[(possible_points.size()-1)/2],ForwardOrBack);
                //recursively, according to the sample just generated, find the next possible one along the same direction
            }
        }
    }
    //if there is no possible points at next_x, stop the sample process along this direction and do not call another auto_find_next
}

void GraphDigitiser::toggleSceneActions(bool &sceneCheck)
{
    ui->actionLog_Log->setEnabled(sceneCheck);
    ui->actionLinear_log->setEnabled(sceneCheck);
    ui->actionlog_Linear->setEnabled(sceneCheck);
    ui->actionLinear_linear->setEnabled(sceneCheck);
    ui->actionCalibrate->setEnabled(sceneCheck);
    ui->actionAjust_Scale->setEnabled(sceneCheck);
}//set these actions according to whether there is an image set as the scene or not

void GraphDigitiser::toggleAfterCalibrationActions(bool &CalibrationFinished)
{
    ui->actionAuto_digitise->setEnabled(CalibrationFinished);
    ui->actionManually_digitise->setEnabled(CalibrationFinished);
}//set these function according to whether the calibration is finished or not


void GraphDigitiser::on_actionOpen_Image_triggered()
{
    if(sceneCheck){
        //if an image has already been set as the scene
        auto result=QMessageBox::question(this,tr("Warning!"),tr("Do you want to leave the work and cover the current image?"),QMessageBox::Yes | QMessageBox::No | QMessageBox::Save,QMessageBox::No);
        QString file;
        QPixmap graph;
        switch(result){
            case QMessageBox::Yes:
                scene->clear();
                sceneCheck=false;
                calibrationFinished=false;
                isManual=false;
                isAuto=false;
                toggleAfterCalibrationActions(calibrationFinished);
                ui->actionExport_Data->setDisabled(true);
                //initialize when the scene is removed
                file = QFileDialog::getOpenFileName(this,tr("Open file"));
                graph=QPixmap(file);
                scene->addPixmap(graph);
                sceneCheck=true;
                toggleSceneActions(sceneCheck);
                break;
            case QMessageBox::No:
                break;
            case QMessageBox::Save:
                GraphDigitiser::on_actionExport_Data_triggered();
                break;
            default:
                break;
        }
        //ask the user to save current work or cancel or give up the work and cover the scene
    }else{
        //if the scene has not been set
        QString file = QFileDialog::getOpenFileName(this,tr("Open file"));
        QPixmap graph(file);
        scene->addPixmap(graph);
        //open the file and set is as the scene
        sceneCheck=true;
        toggleSceneActions(sceneCheck);
        //run the function to enable some actions
    }
}

void GraphDigitiser::on_actionCalibrate_triggered()
{
    isCalibrating=true;//trigger the calibrating process
    calibrationFinished=false;//make it false before the process is finished
    toggleAfterCalibrationActions(calibrationFinished);//make some actions disabled before the calibration finishes
    CalibrationX.clear();//clear the vector to store the 2 points
    CalibrationY.clear();//clear the vector to store the 2 points
    CalibrationCount=0;//re-initialize teh count so that the process can go properly
}

void GraphDigitiser::CalibrateY()
{
    Calibration_factorY=(Ymax-Ymin)/(CalibrationY[1].y()-CalibrationY[0].y());
    //calculate the Y factor
    //qDebug("CalibrationY: %f", Calibration_factorY);
}

void GraphDigitiser::CalibrateX()
{
    Calibration_factorX=(Xmax-Xmin)/(CalibrationX[1].x()-CalibrationX[0].x());
    //calculate the X factor
    //qDebug("CalibrationX: %f", Calibration_factorX);
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
    //since the switch case does not accept QString, convert the mode into int
    qreal pointX=0,pointY=0;
    QVector<QPointF> CalibratedVec;
    //initialize pointX and pointY to store the calculated value and push them back into the vector
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
    //calculate differently according to different modes
    return CalibratedVec;
    //return the vector
}


void GraphDigitiser::on_actionManually_digitise_triggered()
{
    isManual=true;//set this as true to trigger the manual sampling process
    manualSamples.clear();//clear the vector to store samples
    isAuto=false;//disable the auto digitising process
    autoSamples.clear();//clear the vector and avoid any misuse
    ui->actionExport_Data->setDisabled(true);//set exportdata action to disabled before any sample is generated
    ui->graphicsView->viewport()->setCursor(Qt::CrossCursor);//set the cursor to arrowcursor
}

void GraphDigitiser::on_actionLinear_linear_triggered()
{
    mode="LinearLinear";
    XLog=1;
    YLog=1;
}
//set mode as "LinearLinear"

void GraphDigitiser::on_actionLinear_log_triggered()
{
    mode="LinearLog";
    XLog=1;
    YLog=1;
    bool ylog;
    YLog=QInputDialog::getDouble(this,tr("YLog"),tr("Please enter the base of Y axis:"),0,-2147483647,2147483647,1,&ylog);
}
//set mode as "LinearLog" and ask the user for the base of YLog

void GraphDigitiser::on_actionlog_Linear_triggered()
{
    mode="LogLinear";
    XLog=1;
    YLog=1;
    bool xlog;
    XLog=QInputDialog::getDouble(this,tr("XLog"),tr("Please enter the base of X axis:"),0,-2147483647,2147483647,1,&xlog);
}
//set mode as "LogLinear" and ask the user for the base of XLog


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
//set mode as "LogLog" and ask the user for the base of XLog and YLog

void GraphDigitiser::on_actionExport_Data_triggered()
{
    //this action is to save data to file
    if(autoSamples.size()>0||manualSamples.size()>0){
        //if there is any sample to save
        QString fileName=QFileDialog::getSaveFileName(this,tr("Save as"),tr("*.csv"));
        QFile file(fileName);
        //first, get the saving path
        if(autoSamples.size()>0){
            QVector<QPointF> converted;
            for (int i=0;i<autoSamples.size();i++) {
                converted.push_back(QPointF(autoSamples[i]));
            }
            //convert QPoint to QPointF since the auto-sampled samples are QPoint
            //and the function CalculatePos need QPointF
            QVector<QPointF> output = CalculatePos(converted,mode);
            //convert pixel coordinates to real coordinates
            if(file.open(QIODevice::WriteOnly)){
                //if the file is successfully opened
                    QTextStream out(&file);
                    out<<"X,Y\n";
                    for (int i=0;i<output.size();i++) {
                        out<<output[i].x()<<","<<output[i].y()<<"\n";
                    }
                    //write the output
                    file.close();
                    //close the file
                    QMessageBox::information(this,tr("Tip!"),tr("The automatically generated results have been saved!"));
                    //pop up the success message
            }
        }
        else if(manualSamples.size()>0){
            QVector<QPointF> output = CalculatePos(manualSamples,mode);
            //convert pixel coordinates to real coordinates
            if(file.open(QIODevice::WriteOnly)){
                //if the file is successfully opened
                QTextStream out(&file);
                out<<"X,Y\n";
                for (int i=0;i<output.size();i++) {
                    out<<output[i].x()<<","<<output[i].y()<<"\n";
                }
                //write the output
                file.close();
                //close the file
                QMessageBox::information(this,tr("Tip!"),tr("The manually generated results have been saved!"));
                //pop up the success message
            }
        }
    }else{
        QMessageBox::information(this,tr("Tip!"),tr("You haven't done any work!"));
        //if there is not any sample to be export, remind the user
    }
}

void GraphDigitiser::on_actionAjust_Scale_triggered()
{
    //this action is slightly different from the Calibrate action. This action is used for user to ajust the scale after choosing points,
    //in case the user accidentally inputed the wrong Xmin,Xmax,Ymin, or Ymax.
    //I added this action because one of the digitisers I found online contains this function
    bool xmin;
    Xmin=QInputDialog::getDouble(this,tr("Xmin"),tr("Please enter the minimum value on X axis:"),0,-2147483647,2147483647,3,&xmin);
    bool xmax;
    Xmax=QInputDialog::getDouble(this,tr("Xmax"),tr("Please enter the maximum value on X axis:"),0,-2147483647,2147483647,3,&xmax);
    bool ymin;
    Ymin=QInputDialog::getDouble(this,tr("Ymin"),tr("Please enter the minimum value on Y axis:"),0,-2147483647,2147483647,3,&ymin);
    bool ymax;
    Ymax=QInputDialog::getDouble(this,tr("Ymax"),tr("Please enter the maximum value on Y axis:"),0,-2147483647,2147483647,3,&ymax);
    //ask the user to input the new Xmin,Xmax,Ymin, or Ymax.
    CalibrateX();
    CalibrateY();
    //calculate the calibration factor again.
}


void GraphDigitiser::on_actionAuto_digitise_triggered()
{
    isAuto=true;//set this as true to trigger the auto-sample process
    autoSamples.clear();//clear the vector to store samples
    isManual=false;//disable the manual digitising process
    manualSamples.clear();//clear the vector and avoid any misuse
    ui->actionExport_Data->setDisabled(true);//set exportdata action to disabled before any sample is generated
    QMessageBox::information(this,tr("Tip!"),tr("You need to choose a point on the curve to start!"));
    //indicate the user to click once on the curve
    ui->graphicsView->viewport()->setCursor(Qt::ArrowCursor);
    //set the cursor to arrowcursor
}

void GraphDigitiser::on_actionInformation_triggered()
{
    QMessageBox::information(this,tr("Information"),tr("This is a ELEC362 C++ Project \n For the use of digitising 1D graph \n Author: Xueyang Liu "));
    //show the information of this program
}
