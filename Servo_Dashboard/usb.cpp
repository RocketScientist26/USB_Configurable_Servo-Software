#include <QTimer>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parser.h"
#include "usb.h"

extern uint8_t ui_rq_revert_flash;

QSerialPort *usb_port;
QByteArray usb_data_received;
QByteArray usb_data_transmit;
QTimer *usb_detect_timer;
QTimer *usb_rx_timeout_timer;
QTimer *usb_poll_timer;
uint32_t usb_poll_time_ms = 0;

void MainWindow::USB_Init(){
    usb_rx_timeout_timer = new QTimer(this);
    connect(usb_rx_timeout_timer, SIGNAL(timeout()), this, SLOT(USB_RX_Timeout_Timer_Ovf()));
    usb_detect_timer = new QTimer(this);
    connect(usb_detect_timer, SIGNAL(timeout()), this, SLOT(USB_Detect_Timer_Ovf()));
    usb_poll_timer = new QTimer(this);
    connect(usb_poll_timer, SIGNAL(timeout()), this, SLOT(USB_Poll_Timer_Ovf()));
    usb_port = new QSerialPort(this);
    connect(usb_port,SIGNAL(readyRead()),this,SLOT(USB_Data_Received()));
    connect(usb_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(USB_Error(QSerialPort::SerialPortError)));
    USB_Detect_Timer_Ovf();
}
void MainWindow::USB_Detect_Timer_Ovf(){
    usb_detect_timer->stop();
    if(usb_port->isOpen() == false){
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int i = 0;
        int ports_count = ports.size();
        if(ports_count > 0){
            while(i != ports_count){
                if((ports.at(i).vendorIdentifier() == USB_VID)&&(ports.at(i).productIdentifier() == USB_PID)){
                    usb_port->setPortName(ports.at(i).portName());
                    usb_port->setBaudRate(9600);
                    usb_port->setDataBits(QSerialPort::Data8);
                    usb_port->setParity(QSerialPort::NoParity);
                    usb_port->setStopBits(QSerialPort::OneStop);
                    usb_port->setFlowControl(QSerialPort::NoFlowControl);
                    if((usb_port->open(QIODevice::ReadWrite) == true)){
                        USB_Connected();
                        break;
                    }
                }
                i++;
            }
            if(i == ports_count){
                USB_Disconnected();
            }
        }
        else{
            USB_Disconnected();
        }
    }
    else{
        USB_Disconnected();
    }
}
void MainWindow::USB_Connected(){
    ui->label_Device_Status->setText("Connected!");
    Parser_Request_Config(PARSER_SETTINGS_KEEP);
}
void MainWindow::USB_Disconnected(){
    if(usb_port->isOpen()){
        usb_port->close();
    }
    UI_Clear();
    usb_rx_timeout_timer->stop();
    usb_detect_timer->stop();
    usb_detect_timer->start(20);
}
void MainWindow::USB_Error(QSerialPort::SerialPortError error){
    if(
        (error == QSerialPort::ResourceError)||
        (error == QSerialPort::UnsupportedOperationError)||
        (error == QSerialPort::NotOpenError)||
        (error == QSerialPort::TimeoutError)||
        (error == QSerialPort::OpenError)||
        (error == QSerialPort::PermissionError)||
        (error == QSerialPort::DeviceNotFoundError)||
        (error == QSerialPort::UnknownError)
    ){
        USB_Disconnected();
    }
}
void MainWindow::USB_Poll_Timer_Ovf(){
    usb_poll_timer->stop();
    Parser_Request_Status();
}
void MainWindow::USB_Data_Received(){
    usb_data_received.clear();
    usb_data_received.append(usb_port->readAll());
    if(usb_data_received.at(0) == usb_data_received.length()){
        if(CRC32_Get((uint32_t *)&usb_data_received.data()[0], (usb_data_received.length() / 4) - 1) == *(uint32_t *)&usb_data_received.data()[usb_data_received.length() - 4]){
            usb_poll_time_ms = USB_RX_TIMEOUT_MS - usb_rx_timeout_timer->remainingTime();
            usb_rx_timeout_timer->stop();
            switch(usb_data_received.data()[1]){
                case PARSER_CMD_CONFIG:
                    Parser_Parse_Config();
                    UI_Enble(true);
                    on_actionDevice_Signal_Ignore_On_USBAction_triggered();
                    usb_rx_timeout_timer->start(USB_RX_TIMEOUT_MS);
                    if(!ui->pushButton_Device_Pause_Resume->isChecked()){
                        Graph_Clear();
                    }
                    Parser_Request_Status();
                break;
                case PARSER_CMD_STATUS:
                    Parser_Parse_Status();
                    if(ui_rq_revert_flash){
                        usb_poll_timer->stop();
                        usb_rx_timeout_timer->start(USB_RX_TIMEOUT_MS);
                        Parser_Request_Config(ui_rq_revert_flash);
                        ui_rq_revert_flash = PARSER_SETTINGS_KEEP;
                    }else{
                        usb_poll_timer->stop();
                        usb_rx_timeout_timer->start(USB_RX_TIMEOUT_MS);
                        usb_poll_timer->start(USB_STATUS_POLL_MS);
                    }
                break;
                default:
                    USB_Disconnected();
                break;
            }
        }else{
            USB_Disconnected();
        }
    }else{
        USB_Disconnected();
    }
}
void MainWindow::USB_Send(){
    if(usb_port->isOpen()){
        usb_data_transmit.resize(usb_data_transmit.length() + 4);
        *(uint32_t *)&usb_data_transmit.data()[usb_data_transmit.length() - 4] = CRC32_Get((uint32_t *)&usb_data_transmit.data()[0], ((uint32_t)usb_data_transmit.data()[0] / 4) - 1);
        usb_port->write(usb_data_transmit);
    }
}
void MainWindow::USB_RX_Timeout_Timer_Ovf(){
    USB_Disconnected();
    ui->label_Device_Status->setText("USB Error!");
}
