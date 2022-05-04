#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parser.h"
#include <QTimer>
#include <QDesktopServices>

uint8_t ui_rq_revert_flash = 0;
uint8_t ui_oscillate_stage = 0;
extern uint16_t parser_potentiometer_current_step;

QTimer *pid_test_oscillate_timer;

void MainWindow::UI_Init(){
    pid_test_oscillate_timer = new QTimer(this);
    connect(pid_test_oscillate_timer, SIGNAL(timeout()), this, SLOT(UI_PID_Test_Oscillate_Timer_Overflowed()));
}
void MainWindow::UI_Clear(){
    ui->label_Device_Status->setText("Disconnected!");
    Graph_Clear();
    ui->checkBox_Device_Ignore_Signal_On_USB->setCheckable(true);
    ui->doubleSpinBox_PID_P_1->setValue(1);
    ui->doubleSpinBox_PID_I_1->setValue(0);
    ui->doubleSpinBox_PID_D_1->setValue(3);
    ui->doubleSpinBox_PID_P_2->setValue(1);
    ui->doubleSpinBox_PID_I_2->setValue(0);
    ui->doubleSpinBox_PID_D_2->setValue(3);
    ui->doubleSpinBox_PID_P_3->setValue(1);
    ui->doubleSpinBox_PID_I_3->setValue(0);
    ui->doubleSpinBox_PID_D_3->setValue(3);
    ui->doubleSpinBox_PID_Split_1->setValue(25);
    ui->doubleSpinBox_PID_Split_2->setValue(5);
    ui->spinBox_PID_Sampling_Hz->setValue(200);
    ui->checkBox_PID_Pon->setChecked(false);
    ui->checkBox_PID_Running->setChecked(false);
    ui->doubleSpinBox_PID_Test_Pos_A->setValue(25);
    ui->doubleSpinBox_PID_Test_Pos_B->setValue(75);
    ui->spinBox_PID_Test_Time->setValue(5);
    ui->radioButton_LED_Signal->setChecked(true);
    ui->doubleSpinBox_Signal_Length->setValue(2);
    ui->doubleSpinBox_Signal_Timeout->setValue(18);
    ui->doubleSpinBox_Signal_Test_Position->setValue(50);
    ui->checkBox_Motor_Invert->setChecked(false);
    ui->horizontalSlider_Motor_Max_Power->setValue(100);
    ui->label_Motor_Max_Power_Val->setText("100%");
    ui->label_Potentiometer_Current_Step->setText("Current step: 0");
    ui->dial_Potentiometer->setValue(128);
    ui->spinBox_Potentiometer_Start_Margin->setValue(128);
    ui->spinBox_Potentiometer_End_Margin->setValue(3967);
    ui->pushButton_Signal_Test_Hold->setChecked(false);
    ui->pushButton_PID_Test_Oscillate->setChecked(false);
    ui->pushButton_Device_Pause_Resume->setChecked(false);
    UI_Enble(false);
}
void MainWindow::UI_Enble(bool enable){
    ui->groupBox_Potentiometer->setEnabled(enable);
    ui->groupBox_Motor->setEnabled(enable);
    ui->groupBox_Signal->setEnabled(enable);
    ui->groupBox_LED->setEnabled(enable);
    ui->groupBox_PID->setEnabled(enable);
    ui->checkBox_Device_Ignore_Signal_On_USB->setEnabled(enable);
    ui->actionUpdate_flash->setEnabled(enable);
    ui->actionRevert_flash->setEnabled(enable);
    ui->pushButton_Device_Pause_Resume->setEnabled(enable);
    ui->actionLoad_configuration->setEnabled(enable);
    ui->actionSave_Configuration_to_file->setEnabled(enable);
}

void MainWindow::UI_PID_Test_Oscillate_Timer_Overflowed(){
    pid_test_oscillate_timer->stop();
    pid_test_oscillate_timer->start(ui->spinBox_PID_Test_Time->value() * 1000);
    ui_oscillate_stage = 1 - ui_oscillate_stage;
}

void MainWindow::on_actionPotentiometer_Set_StartAction_triggered(){
    if(parser_potentiometer_current_step > (ui->spinBox_Potentiometer_End_Margin->value() - 128)){
        ui->spinBox_Potentiometer_Start_Margin->setValue(ui->spinBox_Potentiometer_End_Margin->value() - 128);
    }else{
        ui->spinBox_Potentiometer_Start_Margin->setValue(parser_potentiometer_current_step);
    }
}
void MainWindow::on_actionPotentiometer_Set_EndAction_triggered(){
    if(parser_potentiometer_current_step < (ui->spinBox_Potentiometer_Start_Margin->value() + 128)){
        ui->spinBox_Potentiometer_End_Margin->setValue(ui->spinBox_Potentiometer_Start_Margin->value() + 128);
    }else{
        ui->spinBox_Potentiometer_End_Margin->setValue(parser_potentiometer_current_step);
    }
}
void MainWindow::on_actionPotentiometer_Start_margin_SpinboxAction_triggered(){
    if(ui->spinBox_Potentiometer_End_Margin->value() < ui->spinBox_Potentiometer_Start_Margin->value() + 128){
        ui->spinBox_Potentiometer_End_Margin->setValue(ui->spinBox_Potentiometer_Start_Margin->value() + 128);
    }
}
void MainWindow::on_actionPotentiometer_End_margin_SpinboxAction_triggered(){
    if(ui->spinBox_Potentiometer_Start_Margin->value() > ui->spinBox_Potentiometer_End_Margin->value() - 128){
        ui->spinBox_Potentiometer_Start_Margin->setValue(ui->spinBox_Potentiometer_End_Margin->value() - 128);
    }
}
void MainWindow::on_actionMotor_Max_Power_ChangedAction_triggered(){
    ui->label_Motor_Max_Power_Val->setText(QString::number(ui->horizontalSlider_Motor_Max_Power->value()).append("%"));
}
void MainWindow::on_actionPID_Test_OscillateAction_triggered(){
    if(ui->pushButton_PID_Test_Oscillate->isChecked()){
        pid_test_oscillate_timer->start(ui->spinBox_PID_Test_Time->value() * 1000);
        ui->checkBox_Device_Ignore_Signal_On_USB->setEnabled(false);
        ui->pushButton_Signal_Test_Hold->setEnabled(false);
        ui->pushButton_Motor_Test_Backward->setEnabled(false);
        ui->pushButton_Motor_Test_Forward->setEnabled(false);
        ui->actionUpdate_flash->setEnabled(false);
        ui->actionRevert_flash->setEnabled(false);
        ui->actionLoad_configuration->setEnabled(false);
    }else{
        pid_test_oscillate_timer->stop();
        ui_oscillate_stage = 0;
        ui->checkBox_Device_Ignore_Signal_On_USB->setEnabled(true);
        ui->pushButton_Signal_Test_Hold->setEnabled(true);
        ui->pushButton_Motor_Test_Backward->setEnabled(true);
        ui->pushButton_Motor_Test_Forward->setEnabled(true);
        ui->actionUpdate_flash->setEnabled(true);
        ui->actionRevert_flash->setEnabled(true);
        ui->actionLoad_configuration->setEnabled(true);
    }
}
void MainWindow::on_actionUpdate_flash_triggered(){
    UI_Enble(false);
    ui_rq_revert_flash = PARSER_SETTINGS_FLASH;
}
void MainWindow::on_actionRevert_flash_triggered(){
    UI_Enble(false);
    ui_rq_revert_flash = PARSER_SETTINGS_REVERT;
}
void MainWindow::on_actionSignal_Test_HoldAction_triggered(){
    if(ui->pushButton_Signal_Test_Hold->isChecked()){
        ui->checkBox_Device_Ignore_Signal_On_USB->setEnabled(false);
        ui->pushButton_PID_Test_Oscillate->setEnabled(false);
        ui->pushButton_Motor_Test_Backward->setEnabled(false);
        ui->pushButton_Motor_Test_Forward->setEnabled(false);
        ui->actionUpdate_flash->setEnabled(false);
        ui->actionRevert_flash->setEnabled(false);
        ui->actionLoad_configuration->setEnabled(false);
    }else{
        ui->checkBox_Device_Ignore_Signal_On_USB->setEnabled(true);
        ui->pushButton_PID_Test_Oscillate->setEnabled(true);
        ui->pushButton_Motor_Test_Backward->setEnabled(true);
        ui->pushButton_Motor_Test_Forward->setEnabled(true);
        ui->actionUpdate_flash->setEnabled(true);
        ui->actionRevert_flash->setEnabled(true);
        ui->actionLoad_configuration->setEnabled(true);
    }
}
void MainWindow::on_actionDevice_Signal_Ignore_On_USBAction_triggered(){
    if(ui->checkBox_Device_Ignore_Signal_On_USB->isChecked()){
        ui->actionUpdate_flash->setEnabled(true);
        ui->actionRevert_flash->setEnabled(true);
        ui->actionLoad_configuration->setEnabled(true);
        ui->pushButton_Motor_Test_Backward->setEnabled(true);
        ui->pushButton_Motor_Test_Forward->setEnabled(true);
        ui->pushButton_Signal_Test_Hold->setEnabled(true);
        ui->pushButton_PID_Test_Oscillate->setEnabled(true);
    }else{
        ui->actionUpdate_flash->setEnabled(false);
        ui->actionRevert_flash->setEnabled(false);
        ui->actionLoad_configuration->setEnabled(false);
        ui->pushButton_Motor_Test_Backward->setEnabled(false);
        ui->pushButton_Motor_Test_Forward->setEnabled(false);
        ui->pushButton_Signal_Test_Hold->setEnabled(false);
        ui->pushButton_PID_Test_Oscillate->setEnabled(false);
        ui->pushButton_Signal_Test_Hold->setChecked(false);
        ui->pushButton_PID_Test_Oscillate->setChecked(false);
    }
}
void MainWindow::on_actionAbout_triggered(){
    QMessageBox aboutbox(this);
    aboutbox.setIconPixmap(QPixmap(":/about.png"));//TBD
    aboutbox.setWindowTitle("About");
    aboutbox.setText("\n"
                     "Servo Dashboard - Variant \"A\"\n"
                     "\n"
                     "This software is a part of an\n"
                     "open source servo motor project.\n"
                     "\n"
                     "www.Rocket-Scientist.me"
                     "\n");
    aboutbox.exec();
}
void MainWindow::on_actionManual_triggered(){
    QDesktopServices::openUrl(QUrl::fromLocalFile("Manual.pdf"));
}
