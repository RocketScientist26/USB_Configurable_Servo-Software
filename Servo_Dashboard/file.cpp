#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#define FILE_CRC_PADDING char(0)
#define FILE_LENGTH 74

QFile file;
QByteArray file_data;

void MainWindow::on_actionSave_Configuration_to_file_triggered(){
    QString filename = QFileDialog::getSaveFileName(this,tr("Save configuration as..."), "Configuration",tr("Servo binary conf. (*.sbc);;All Files (*)"));
    if(filename.length() != 0){
        file.setFileName(filename);
        if(file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered)){
            file_data.clear();

            //Append file length
            uint16_t file_length = FILE_LENGTH;
            file_data.append(&(*(char *)&file_length), 2);
            //Append values
            uint8_t file_tmp[4] = {0,0,0,0};
            *(uint16_t *)&file_tmp[0] = (uint16_t)ui->spinBox_Potentiometer_Start_Margin->value();
            file_data.append(&(*(char *)&file_tmp[0]), 2);
            *(uint16_t *)&file_tmp[0] = (uint16_t)ui->spinBox_Potentiometer_End_Margin->value();
            file_data.append(&(*(char *)&file_tmp[0]), 2);
            if(ui->checkBox_Motor_Invert->isChecked()){
                file_data.append(char(1));
            }else{
                file_data.append(char(0));
            }
            file_data.append(char(ui->horizontalSlider_Motor_Max_Power->value()));
            if(ui->checkBox_Device_Ignore_Signal_On_USB->isChecked()){
                file_data.append(char(1));
            }else{
                file_data.append(char(0));
            }
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_Signal_Min->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_Signal_Max->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(uint32_t *)&file_tmp[0] = (uint32_t)ui->spinBox_Signal_Timeout->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            if(ui->radioButton_LED_Off->isChecked()){
                file_data.append(char(0));
            }else if(ui->radioButton_LED_Power->isChecked()){
                file_data.append(char(1));
            }else if(ui->radioButton_LED_Signal->isChecked()){
                file_data.append(char(2));
            }else if(ui->radioButton_LED_Position_Change->isChecked()){
                file_data.append(char(3));
            }
            if(ui->checkBox_PID_Pon->isChecked()){
                file_data.append(char(0));
            }else{
                file_data.append(char(1));
            }
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_P_1->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_I_1->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_D_1->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_P_2->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_I_2->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_D_2->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_P_3->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_I_3->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_D_3->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_Split_1->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(float *)&file_tmp[0] = (float)ui->doubleSpinBox_PID_Split_2->value();
            file_data.append(&(*(char *)&file_tmp[0]), 4);
            *(uint16_t *)&file_tmp[0] = (uint16_t)ui->spinBox_PID_Sampling_Hz->value();
            file_data.append(&(*(char *)&file_tmp[0]), 2);
            file_data.append(FILE_CRC_PADDING);

            uint32_t file_crc = CRC32_Get((uint32_t *)&file_data.data()[0], file_data.length() / 4);
            file_data.append(&(*(char *)&file_crc), 4);
            file.resize(0);
            file.write(file_data);

            file.close();
        }else{
            //File save error message
            QMessageBox aboutbox(this);
            aboutbox.setWindowTitle("Error");
            aboutbox.setText("Error saving configuration file!");
            aboutbox.exec();
        }
    }
}
void MainWindow::on_actionLoad_configuration_triggered(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Load configuraiton from file..."), "",tr("Servo binary conf. (*.sbc);;All Files (*)"));
    if(filename.length() != 0){
        file.setFileName(filename);
        if(file.open(QIODevice::ReadWrite | QIODevice::Unbuffered)){
            file_data.clear();
            file_data.append(file.readAll());
            uint32_t file_crc = *(uint32_t *)&file_data.data()[file.size()-4];
            if(file_crc != CRC32_Get((uint32_t *)&file_data.data()[0], (file_data.length() / 4 ) - 1)){
                file.close();
                //File wrong CRC error message
                QMessageBox aboutbox(this);
                aboutbox.setWindowTitle("Error");
                aboutbox.setText("Error opening configuration file!\n"
                                 "Wrong CRC!");
                aboutbox.exec();
            }else if((*(uint16_t *)&file_data.data()[0] != FILE_LENGTH) || (file_data.length() != FILE_LENGTH)){
                file.close();
                //Wrong file size message
                QMessageBox aboutbox(this);
                aboutbox.setWindowTitle("Error");
                aboutbox.setText("Error opening configuration file!\n"
                                 "Wrong data size!");
                aboutbox.exec();
            }else{
                //Set UI values from data
                ui->spinBox_Potentiometer_Start_Margin->setValue(*(uint16_t *)&file_data.data()[2]);
                ui->spinBox_Potentiometer_End_Margin->setValue(*(uint16_t *)&file_data.data()[4]);
                if(file_data.at(6)){
                    ui->checkBox_Motor_Invert->setChecked(true);
                }else{
                    ui->checkBox_Motor_Invert->setChecked(false);
                }
                ui->horizontalSlider_Motor_Max_Power->setValue((uint8_t)file_data.at(7));
                if(file_data.at(8)){
                    ui->checkBox_Device_Ignore_Signal_On_USB->setChecked(true);
                }else{
                    ui->checkBox_Device_Ignore_Signal_On_USB->setChecked(false);
                }
                ui->doubleSpinBox_Signal_Min->setValue(*(float *)&file_data.data()[9]);
                ui->doubleSpinBox_Signal_Max->setValue(*(float *)&file_data.data()[13]);
                ui->spinBox_Signal_Timeout->setValue(*(uint32_t *)&file_data.data()[17]);
                switch(file_data.at(21)){
                    case 0:
                        ui->radioButton_LED_Off->setChecked(true);
                        ui->radioButton_LED_Power->setChecked(false);
                        ui->radioButton_LED_Signal->setChecked(false);
                        ui->radioButton_LED_Position_Change->setChecked(false);
                    break;
                    case 1:
                        ui->radioButton_LED_Off->setChecked(false);
                        ui->radioButton_LED_Power->setChecked(true);
                        ui->radioButton_LED_Signal->setChecked(false);
                        ui->radioButton_LED_Position_Change->setChecked(false);
                    break;
                    case 2:
                        ui->radioButton_LED_Off->setChecked(false);
                        ui->radioButton_LED_Power->setChecked(false);
                        ui->radioButton_LED_Signal->setChecked(true);
                        ui->radioButton_LED_Position_Change->setChecked(false);
                    break;
                    case 3:
                        ui->radioButton_LED_Off->setChecked(false);
                        ui->radioButton_LED_Power->setChecked(false);
                        ui->radioButton_LED_Signal->setChecked(false);
                        ui->radioButton_LED_Position_Change->setChecked(true);
                    break;
                }
                if(file_data.at(22)){
                    ui->checkBox_PID_Pon->setChecked(false);
                }else{
                    ui->checkBox_PID_Pon->setChecked(true);
                }
                ui->doubleSpinBox_PID_P_1->setValue(*(float *)&file_data.data()[23]);
                ui->doubleSpinBox_PID_I_1->setValue(*(float *)&file_data.data()[27]);
                ui->doubleSpinBox_PID_D_1->setValue(*(float *)&file_data.data()[31]);
                ui->doubleSpinBox_PID_P_2->setValue(*(float *)&file_data.data()[35]);
                ui->doubleSpinBox_PID_I_2->setValue(*(float *)&file_data.data()[39]);
                ui->doubleSpinBox_PID_D_2->setValue(*(float *)&file_data.data()[43]);
                ui->doubleSpinBox_PID_P_3->setValue(*(float *)&file_data.data()[47]);
                ui->doubleSpinBox_PID_I_3->setValue(*(float *)&file_data.data()[51]);
                ui->doubleSpinBox_PID_D_3->setValue(*(float *)&file_data.data()[55]);
                ui->doubleSpinBox_PID_Split_1->setValue(*(float *)&file_data.data()[59]);
                ui->doubleSpinBox_PID_Split_2->setValue(*(float *)&file_data.data()[63]);
                ui->spinBox_PID_Sampling_Hz->setValue(*(uint16_t *)&file_data.data()[67]);
            }
            file.close();
        }else{
            //File open error message
            QMessageBox aboutbox(this);
            aboutbox.setWindowTitle("Error");
            aboutbox.setText("Error opening configuration file!");
            aboutbox.exec();
        }
    }
}
