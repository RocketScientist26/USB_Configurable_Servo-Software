#include "usb.h"

/*!
    Constructor, connects signals and runs device detection timer
*/
Usb::Usb()
{
    //Timers
    connect(detect_timer, SIGNAL(timeout()), this, SLOT(detectTimerOvf()));
    connect(rx_timer, SIGNAL(timeout()), this, SLOT(disconnect()));
    rx_timer->setTimerType(Qt::PreciseTimer);

    //Port
    connect(port,SIGNAL(readyRead()),this,SLOT(dataReceived()));
    connect(port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(portError(QSerialPort::SerialPortError)));
}

/*!
    Runs timer for continuous checking for available devices to connect
*/
void Usb::run()
{
    detect_timer->start(1);
}

/*!
    Prepends length to QByteArray, appends padding symbols + CRC at the end and sends if port is connected. Runs responce receive timeout timer
*/
void Usb::send(QByteArray data)
{
    if(port->isOpen()){
        //Start timer for measuring total packet time
        rx_timer->start(RX_TIMEOUT_MS);

        //Make "data length + 1" even of "sizeof(uint32_t)" for 32 bit CRC
        while((data.length() + 1) % sizeof(uint32_t)){
            data.append(1, USB_CRC_PADDING_SYMBOL);
        }

        //Assemble packet
        QByteArray data_tx =
            QByteArray(1, 1 + (uint8_t)data.length() + sizeof(uint32_t)).   //Append length (length byte + data size + CRC size)
            append(data)                                                    //Append data
        ;
        //Append CRC of length and data together
        uint32_t crc = Crc32().get((uint32_t *)&data_tx.data()[0], (uint32_t)data_tx.length() / sizeof(uint32_t));
        data_tx.append(QByteArray((char *)&crc, sizeof(uint32_t)));         //Append CRC

        //Send
        port->write(data_tx);
    }
}

/*!
    Polls for available ports and connects if possible, runs in every 1ms via timer
*/
void Usb::detectTimerOvf()
{
    if(!port->isOpen())
    {
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int i = 0;
        int ports_count = ports.size();
        if(ports_count > 0)
        {
            while(i != ports_count)
            {
                if((ports.at(i).vendorIdentifier() == VID) && (ports.at(i).productIdentifier() == PID))
                {
                    port->setPortName(ports.at(i).portName());
                    if(port->open(QIODevice::ReadWrite))
                    {
                        emit connected();
                        return;
                    }
                }
                i++;
            }
        }
    }
}

/*!
    Called on data receive, if data size and CRC are correct "dataReady()" signal is emitted
*/
void Usb::dataReceived()
{
    QByteArray data = port->readAll();

    //If there is any data available
    if(data.length())
    {
        //If length is correct
        if(data.at(0) == data.length())
        {
            //If CRC is correct
            if(Crc32().get((uint32_t *)&data.data()[0], (data.length() / 4) - 1) == *(uint32_t *)&data.data()[data.length() - 4])
            {
                int rx_time = RX_TIMEOUT_MS - rx_timer->remainingTime();
                rx_timer->stop();
                emit dataReady(data, rx_time);
                return;
            }
        }
    }

    disconnect();
}

/*!
    Port error interrupt, just calls "disconnect()" function
*/
void Usb::portError(QSerialPort::SerialPortError error)
{
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
        disconnect();
    }
}

/*!
    Private function, disconnects and emits signal, also called on receive timeout
*/
void Usb::disconnect()
{
    if(port->isOpen()) port->close();
    rx_timer->stop();

    emit disconnected();
}
