#ifndef SERVOGUI_H
#define SERVOGUI_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
class ServoGui;
}
QT_END_NAMESPACE

class ServoGui : public QMainWindow
{
    Q_OBJECT

public:
    ServoGui(QWidget *parent = nullptr);
    ~ServoGui();

private slots:
    void on_degree0_clicked();
    void on_degree45_clicked();
    void readSerial();
    void on_degree90_clicked();
    void updateServo(QString str);


   // void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

private:
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsItem *rect;
    float currAngle;
    const float r;
    const float angleOffset;
    float t_up;
    float t_lo;
    QPolygonF triangle;
    QGraphicsPolygonItem* needle;
    QSerialPort *arduino;
    Ui::ServoGui *ui;
    QString servoSetting;
    QSerialPort *arduino;
    static const quint16 arduino_uno_vendorID = 9025;
    static const quint16 arduino_uno_productID = 67;
    QString arduino_port_name;
    bool arduino_available;

};
#endif // SERVOGUI_H
