//! Class reads and stores servo configuration data into file

#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include "crc32.h"

class File : public QObject
{
    Q_OBJECT
    public:
        //Padding symbol to make 32 bit CRC work correctly
        #define FILE_CRC_PADDING_SYMBOL             uint8_t(0)

        //Error message box title
        #define MESSAGE_ERROR_TITLE                 "Error"
        //File error texts
        #define MESSAGE_ERROR_TEXT_OPEN             "Error opening configuration file!"
        #define MESSAGE_ERROR_TEXT_OPEN_WRONG_CRC   "Error reading configuration file!\n" "Wrong CRC!"
        #define MESSAGE_ERROR_TEXT_OPEN_WRONG_SIZE  "Error reading configuration file!\n" "Wrong data size!"
        #define MESSAGE_ERROR_TEXT_SAVE             "Error writing configuration file!"

        //File data
        typedef struct{
            uint16_t pot_start;
            uint16_t pot_end;
            bool motor_inv;
            uint8_t motor_max;
            bool signal_usb_ignore;
            float signal_min;
            float signal_max;
            uint8_t signal_timeout;
            uint8_t led_mode;
            bool pid_p_on_m;
            float pid_p1;
            float pid_i1;
            float pid_d1;
            float pid_p2;
            float pid_i2;
            float pid_d2;
            float pid_p3;
            float pid_i3;
            float pid_d3;
            float pid_split_1;
            float pid_split_2;
            uint16_t pid_hz;
        }__attribute__((__packed__)) file_data_t;

        bool get(file_data_t *values);
        bool save(file_data_t *values);

    private:
        void showMessage(QString message);
};

#endif // FILE_H
