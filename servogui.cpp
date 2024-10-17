#include "servogui.h"
#include "ui_servogui.h"

ServoGui::ServoGui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServoGui), r(445.0), angleOffset(0.05)
{
    ui->setupUi(this);

    // load bg image
    scene = new QGraphicsScene(this);
    ui->graphicsView.setScene(scene);
    pix = QPixmap();
    scene->addPixmap(pix);

    // setup serialport
    arduino = new QSerialPort;
    arduino_available = false;
    arduino_port_name = "";

    //initialize needle at 0 degrees
    QPen blackpen(Qt::black);
    QBrush greenbrush(Qt::green);
    t_up = angleOffset;
    t_lo = -angleOffset;
    triangle.append(QPointF(r*qCos(t_up) + 505,-r*qsin(t_up) + 495));
    triangle.append(QPointF(505,495));
    triangle.append(QPointF(r*qCos(t_lo) + 505,-r*qsin(t_lo) + 495));
    needle = scene->addPolygon(triangle,blackpen,greenbrush);
    needle->setOpacity(0.3);


    // view ports in System
    qDebug() << "Number of Ports: " << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
             qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
             qDebug() << "Product ID: " << serialPortInfo.hasProductIdentifier();

        }
    }

    // check for ports on arduino
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.productIdentifier() == arduino_uno_productID){
                if(serialPortInfo.vendorIdentifier() == arduino_uno_vendorID){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_available = true;
                    qDebug() << "Port available!!!";
                }

            }
        }
    }

    // setup the port if available
    if(arduino_available){
        // setup communication configure port
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        // update Port through slot
      //  QObject::connect(arduino, SIGNAL(readyRead()),this, SLOT(readSerial()));
    }else{
        QMessageBox::warning(this,"Port Not Found", "Couldn't find Arduino");
    }
}

void ServoGui::readSerial(){
    QPen blackpen(Qt::black);
    QBrush greenbrush(Qt::green);
    QStringList bufferSplit = serialBuffer.split(",");
    if(bufferSplit.length() < 3){
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
    }else{
        ui->range.setText(bufferSplit[1]);
        currAngle = ui->angle.text().toFloat();
        currAngle = qDegreesToRadians(currAngle);
        float dist = bufferSplit[1].toFloat();
        if(dist < 100){
            float radius = dist * 4.5;
            float x = radius * qCos(currAngle);
            float x = radius * qSin(currAngle);
            float xT = x + 505;
            float yT = -1*y + 495;
            rect = scene->addRect(xT,yT,12,12,blackpen,greenbrush);
            rect->setOpacity(0.75);
        }
        serialBuffer = "";
    }

}
ServoGui::~ServoGui()
{
    if (arduino->isOpen()){
        qDebug() << "Closing Port";
        arduino->close();
    }
    delete ui;
}

void ServoGui::on_degree0_clicked()
{
    servoSetting = "0";
    ui->horizontalSlider->setValue(0);
  //  ui->verticalSlider.setValue(0);
    updateServo(servoSetting);
}


void ServoGui::on_degree45_clicked()
{
    servoSetting = "45";
    ui->horizontalSlider->setValue(45);
  //  ui->verticalSlider.setValue(45);
    updateServo(servoSetting);
}


void ServoGui::on_degree90_clicked()
{
    servoSetting = "90";
    ui->horizontalSlider->setValue(90);
   // ui->verticalSlider.setValue(90);
    updateServo(servoSetting);
}


void ServoGui::updateServo(QString str){
    // get the angle and update needle
    currAngle = qDegreesToRadians(command.ToFloat());
    t_up = currAngle + angleOffset;
    t_lo = currAngle - angleOffset;
    triangle[0] = QPointF(r*qCos(t_up) + 505,-r*qsin(t_up) + 495);
    triangle[2] = (QPointF(r*qCos(t_up) + 505,-r*qsin(t_up) + 495);
    needle->setPolygon(triangle);
    float temp = command.toFloat()/1.5;
    QString commandNew = QString::number(temp);
    if(arduino->isWritable()){
        qDebug() << str.toStdString().c_str();
        arduino->write(str.toStdString().c_str());
    }else{
        qDebug() << "Could not write to Servo!!!";
    }
}

void ServoGui::on_horizontalSlider_valueChanged(int value)
{
    QString servoSetting_str= QString::number(value);
    ui->labelConfig->setText(servoSetting_str);
    updateServo(servoSetting_str);

}

