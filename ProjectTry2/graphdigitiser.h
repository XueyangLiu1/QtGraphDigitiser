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
    void on_actionOpen_Image_triggered(bool checked);

private:
    Ui::GraphDigitiser *ui;
    QGraphicsScene *scene = new QGraphicsScene();

    QVector<QPointF> CalibrationX;
    bool isCalibratingX=false;
    QVector<QPointF> CalibrationY;
    bool isCalibratingY=false;

    QVector<QPointF> Samples;
    bool isSampling=false;

};

#endif // GRAPHDIGITISER_H
