//! Class manages sending/receiving packets over USB Virtual COM Port
/*!
    Usb packet structure:

    [TOTAL_LENGHT_INCLUDING_THIS_BYTE_AND_CRC 1][DATA ***][CRC 4]
*/

#ifndef USB_H
#define USB_H

#include <QObject>
#include <QTimer>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "Drivers/crc32.h"

class Usb : public QObject
{
    Q_OBJECT
    public:
        //Padding byte for 32bit CRC
        uint8_t USB_CRC_PADDING_SYMBOL = 0;

        enum{
            VID = 0x0483,           //Class automatically connects to the first detected device with these VID and PID
            PID = 0x5740,           //Device PID
            RX_TIMEOUT_MS = 2000    //If don't get any responce packet for this time, try reconnecting (taken large value to prevent unnecessaryly frequent reconnecting)
        };

        explicit Usb();
        void run();
        void send(QByteArray data);

    signals:
        void connected();
        void disconnected();
        void dataReady(QByteArray data, int packet_time_ms);

    private:
        QSerialPort port;
        QTimer detect_timer;        //Timer for checking available ports list and connecting if applicable
        QTimer rx_timer;            //Timer for measuring packet responce time, also for reconnecting if timed out

    private slots:
        void disconnect();
        void detectTimerOvf();
        void dataReceived();
        void portError(QSerialPort::SerialPortError error);
};

#endif // USB_H
