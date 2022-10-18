#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parser.h"

extern QByteArray usb_data_received;
extern QByteArray usb_data_transmit;

extern uint8_t ui_oscillate_stage;
uint16_t parser_potentiometer_current_step = 0;

void MainWindow::Parser_Request_Config(uint8_t revert_flash){
    usb_data_transmit.clear();
    usb_data_transmit.append(PARSER_TX_CONFIG_LENGTH);
    usb_data_transmit.append(PARSER_CMD_CONFIG);
    usb_data_transmit.append(revert_flash);
    usb_data_transmit.append(PARSER_CRC_PADDING);
    USB_Send();
}
void MainWindow::Parser_Parse_Config(){
    ui->spinBox_Potentiometer_Start_Margin->setValue(*(uint16_t *)&usb_data_received.data()[2]);
    ui->spinBox_Potentiometer_End_Margin->setValue(*(uint16_t *)&usb_data_received.data()[4]);
    ui->horizontalSlider_Motor_Max_Power->setValue(usb_data_received.at(6));
    if(usb_data_received.at(7)){
        ui->checkBox_Motor_Invert->setChecked(true);
    }else{
        ui->checkBox_Motor_Invert->setChecked(false);
    }
    if(usb_data_received.at(8)){
        ui->checkBox_Device_Ignore_Signal_On_USB->setChecked(true);
    }else{
        ui->checkBox_Device_Ignore_Signal_On_USB->setChecked(false);
    }
    ui->doubleSpinBox_Signal_Length->setValue(*(float *)&usb_data_received.data()[9]);
    ui->spinBox_Signal_Timeout->setValue((int)(*(uint32_t *)&usb_data_received.data()[13]));
    switch(usb_data_received.at(17)){
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
    if(usb_data_received.at(18)){
        ui->checkBox_PID_Pon->setChecked(false);
    }else{
        ui->checkBox_PID_Pon->setChecked(true);
    }
    ui->doubleSpinBox_PID_P_1->setValue(*(float *)&usb_data_received.data()[19]);
    ui->doubleSpinBox_PID_I_1->setValue(*(float *)&usb_data_received.data()[23]);
    ui->doubleSpinBox_PID_D_1->setValue(*(float *)&usb_data_received.data()[27]);
    ui->doubleSpinBox_PID_P_2->setValue(*(float *)&usb_data_received.data()[31]);
    ui->doubleSpinBox_PID_I_2->setValue(*(float *)&usb_data_received.data()[35]);
    ui->doubleSpinBox_PID_D_2->setValue(*(float *)&usb_data_received.data()[39]);
    ui->doubleSpinBox_PID_P_3->setValue(*(float *)&usb_data_received.data()[43]);
    ui->doubleSpinBox_PID_I_3->setValue(*(float *)&usb_data_received.data()[47]);
    ui->doubleSpinBox_PID_D_3->setValue(*(float *)&usb_data_received.data()[51]);

    //Convert steps to pecentage
    float single_p = (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f);
    float split_1 = (float)(*(uint16_t *)&usb_data_received.data()[55]) - (float)ui->spinBox_Potentiometer_Start_Margin->value();
    float split_2 = (float)(*(uint16_t *)&usb_data_received.data()[57]) - (float)ui->spinBox_Potentiometer_Start_Margin->value();
    ui->doubleSpinBox_PID_Split_1->setValue(split_1 / single_p);
    ui->doubleSpinBox_PID_Split_2->setValue(split_2 / single_p);

    ui->spinBox_PID_Sampling_Hz->setValue(1000/usb_data_received.at(59));//Convert ms to Hz
}
void MainWindow::Parser_Request_Status(){
    usb_data_transmit.clear();
    usb_data_transmit.append(PARSER_TX_STATUS_LENGTH);
    usb_data_transmit.append(PARSER_CMD_STATUS);
    usb_data_transmit.resize(PARSER_TX_STATUS_LENGTH - 4);
    *(uint16_t *)&usb_data_transmit.data()[2] = ui->spinBox_Potentiometer_Start_Margin->value();
    *(uint16_t *)&usb_data_transmit.data()[4] = ui->spinBox_Potentiometer_End_Margin->value();
    *(uint8_t *)&usb_data_transmit.data()[6] = ui->horizontalSlider_Motor_Max_Power->value();
     if(ui->checkBox_Motor_Invert->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[7] = 1;
    }else{
        *(uint8_t *)&usb_data_transmit.data()[7] = 0;
    }
    if(ui->checkBox_Device_Ignore_Signal_On_USB->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[8] = 1;
    }else{
        *(uint8_t *)&usb_data_transmit.data()[8] = 0;
    }
    *(float *)&usb_data_transmit.data()[9] = ui->doubleSpinBox_Signal_Length->value();
    *(uint32_t *)&usb_data_transmit.data()[13] = ui->spinBox_Signal_Timeout->value();
    if(ui->radioButton_LED_Position_Change->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[17] = 3;
    }else if(ui->radioButton_LED_Signal->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[17] = 2;
    }else if(ui->radioButton_LED_Power->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[17] = 1;
    }else{
        *(uint8_t *)&usb_data_transmit.data()[17] = 0;
    }
    if(ui->checkBox_PID_Pon->isChecked()){
        *(uint8_t *)&usb_data_transmit.data()[18] = 0;
    }else{
        *(uint8_t *)&usb_data_transmit.data()[18] = 1;
    }
    *(float *)&usb_data_transmit.data()[19] = ui->doubleSpinBox_PID_P_1->value();
    *(float *)&usb_data_transmit.data()[23] = ui->doubleSpinBox_PID_I_1->value();
    *(float *)&usb_data_transmit.data()[27] = ui->doubleSpinBox_PID_D_1->value();
    *(float *)&usb_data_transmit.data()[31] = ui->doubleSpinBox_PID_P_2->value();
    *(float *)&usb_data_transmit.data()[35] = ui->doubleSpinBox_PID_I_2->value();
    *(float *)&usb_data_transmit.data()[39] = ui->doubleSpinBox_PID_D_2->value();
    *(float *)&usb_data_transmit.data()[43] = ui->doubleSpinBox_PID_P_3->value();
    *(float *)&usb_data_transmit.data()[47] = ui->doubleSpinBox_PID_I_3->value();
    *(float *)&usb_data_transmit.data()[51] = ui->doubleSpinBox_PID_D_3->value();
    *(uint16_t *)&usb_data_transmit.data()[55] = (uint16_t)(((float)ui->doubleSpinBox_PID_Split_1->value() *
                                                             (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f))
                                                            + (float)ui->spinBox_Potentiometer_Start_Margin->value());
    *(uint16_t *)&usb_data_transmit.data()[57] = (uint16_t)(((float)ui->doubleSpinBox_PID_Split_2->value() *
                                                             (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f))
                                                            + (float)ui->spinBox_Potentiometer_Start_Margin->value());
    *(uint8_t *)&usb_data_transmit.data()[59] = 1000 / ui->spinBox_PID_Sampling_Hz->value();//Convert Hz to ms
    float pos = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_Signal_Test_Position->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
    float pos_a = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_PID_Test_Pos_A->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
    float pos_b = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_PID_Test_Pos_B->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
    if(ui->pushButton_Signal_Test_Hold->isChecked()){
        *(float *)&usb_data_transmit.data()[60] = pos;
        *(uint8_t *)&usb_data_transmit.data()[64] = 1;
    }else if(ui->pushButton_PID_Test_Oscillate->isChecked()){
        if(ui_oscillate_stage == 0){
            *(float *)&usb_data_transmit.data()[60] = pos_a;
        }else{
            *(float *)&usb_data_transmit.data()[60] = pos_b;
        }
        *(uint8_t *)&usb_data_transmit.data()[64] = 1;
    }else{
        *(float *)&usb_data_transmit.data()[60] = 0;
        *(uint8_t *)&usb_data_transmit.data()[64] = 0;
    }
    if(ui->pushButton_Motor_Test_Backward->isDown()){
        *(uint8_t *)&usb_data_transmit.data()[65] = 1;
    }else if(ui->pushButton_Motor_Test_Forward->isDown()){
        *(uint8_t *)&usb_data_transmit.data()[65] = 2;
    }else{
        *(uint8_t *)&usb_data_transmit.data()[65] = 0;
    }
    *(uint8_t *)&usb_data_transmit.data()[66] = PARSER_CRC_PADDING;
    *(uint8_t *)&usb_data_transmit.data()[67] = PARSER_CRC_PADDING;
    USB_Send();
}
void MainWindow::Parser_Parse_Status(){
    if(*(uint8_t *)&usb_data_received.data()[2] == 1){
        ui->checkBox_PID_Running->setChecked(true);
    }else{
        ui->checkBox_PID_Running->setChecked(false);
    }
    Graph_Append(*(float *)&usb_data_received.data()[3], *(float *)&usb_data_received.data()[7], *(int16_t *)&usb_data_received.data()[11]);
    ui->dial_Potentiometer->setValue((uint16_t )*(float *)&usb_data_received.data()[7]);
    parser_potentiometer_current_step = (uint16_t )*(float *)&usb_data_received.data()[7];
    ui->label_Potentiometer_Current_Step->setText(QString("Current step: ").append(QString::number(parser_potentiometer_current_step)));
}
