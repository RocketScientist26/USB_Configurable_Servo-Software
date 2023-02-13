#include "mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->setFixedSize(1251, 772);
    ui->setupUi(this);

    //PID Test oscillation timer
    connect(&pid_test_osc_timer, SIGNAL(timeout()), this, SLOT(pidTestOscTimOvf()));
    pid_test_osc_timer.setTimerType(Qt::PreciseTimer);

    //Chart
    //Load "Latin Modern Mono" monospace font for chart legend
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmono10-regular.otf");
    //Chart itself
    graph = new Graph(ui->chart_Device);
    //Zoom and clip graph to match class variables to mainwindow.ui file configuration, whatever we set there
    on_actionGraphBufferClip_triggered();

    //Parser
    connect(&usb, SIGNAL(dataReady(QByteArray,int)), &parser, SLOT(parseData(QByteArray,int)));
    connect(&parser, SIGNAL(configReceived(Parser::parser_config_t)), this, SLOT(parserConfigReceived(Parser::parser_config_t)));
    connect(&parser, SIGNAL(statusReceived(Parser::parser_rx_status_t,int)), this, SLOT(parserStatusReceived(Parser::parser_rx_status_t,int)));

    //USB
    connect(&usb, SIGNAL(connected()), this, SLOT(usbConnected()));
    connect(&usb, SIGNAL(disconnected()), this, SLOT(usbDisconnected()));
    usb.run();
}
//Destructor
MainWindow::~MainWindow()
{
    delete graph;
    delete ui;
}

/*
    Private slots and functions
*/

//USB connected interrupt from "Usb" object
void MainWindow::usbConnected()
{
    graph->clear();
    usb.send(parser.configRequest(Parser::CMD_SETTINGS_KEEP));

    ui->label_Device_Status->setText("Connected!");
}
//USB disconnected interrupt
void MainWindow::usbDisconnected()
{
    pid_test_osc_timer.stop();
    pid_test_oscillate_edge = 0;
    keep_revert_flash = Parser::CMD_SETTINGS_KEEP;

    ui->pushButton_Signal_Test_Hold->setChecked(false);
    ui->pushButton_PID_Test_Oscillate->setChecked(false);
    uiEnable(&ui_enable[UI_ENABLE_STATE_ALL_DISABLED]);
    ui->label_Device_Status->setText("Disconnected!");
}

//Interrupt from parser if supplied data was identified as configuration data
void MainWindow::parserConfigReceived(Parser::parser_config_t config)
{
    ui->spinBox_Potentiometer_Start_Margin->setValue(config.potentiometer_min);
    ui->spinBox_Potentiometer_End_Margin->setValue(config.potentiometer_max);
    ui->horizontalSlider_Motor_Max_Power->setValue(config.motor_max_power);
    ui->checkBox_Motor_Invert->setChecked(config.motor_invert);
    ui->checkBox_Signal_Ignore_On_USB->setChecked(config.signal_ignore);
    ui->doubleSpinBox_Signal_Min->setValue(config.signal_min);
    ui->doubleSpinBox_Signal_Max->setValue(config.signal_max);
    ui->spinBox_Signal_Timeout->setValue(config.signal_timeout);
    switch(config.led_mode){
        case 0: ui->radioButton_LED_Off->setChecked(true);
        break;
        case 1: ui->radioButton_LED_Power->setChecked(true);
        break;
        case 2: ui->radioButton_LED_Signal->setChecked(true);
        break;
        case 3: ui->radioButton_LED_Position_Change->setChecked(true);
        break;
    }
    ui->checkBox_PID_P_On_M->setChecked(config.pid_on_e_m);
    ui->doubleSpinBox_PID_P_1->setValue(config.pid_kp_1);
    ui->doubleSpinBox_PID_I_1->setValue(config.pid_ki_1);
    ui->doubleSpinBox_PID_D_1->setValue(config.pid_kd_1);
    ui->doubleSpinBox_PID_P_2->setValue(config.pid_kp_2);
    ui->doubleSpinBox_PID_I_2->setValue(config.pid_ki_2);
    ui->doubleSpinBox_PID_D_2->setValue(config.pid_kd_2);
    ui->doubleSpinBox_PID_P_3->setValue(config.pid_kp_3);
    ui->doubleSpinBox_PID_I_3->setValue(config.pid_ki_3);
    ui->doubleSpinBox_PID_D_3->setValue(config.pid_kd_3);

    //Convert steps to pecentage
    float single_p = (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f);
    ui->doubleSpinBox_PID_Split_1->setValue((config.pid_split_steps_1 - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / single_p);
    ui->doubleSpinBox_PID_Split_2->setValue((config.pid_split_steps_2 - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / single_p);

    //Convert ms to Hz
    ui->spinBox_PID_Sampling_Hz->setValue(1000 / config.pid_sampling_ms);

    //Update motor max power value label text
    ui->label_Motor_Max_Power_Val->setText(QString::number(ui->horizontalSlider_Motor_Max_Power->value()) + "%");

    Parser::parser_tx_status_t tx_status = {
        .pid_setpoint = 0,
        .usb_rq_stat_phold = false,
        .usb_rq_stat_motor = false
    };
    usb.send(parser.statusRequest(&config, &tx_status));
}
//Interrupt from parser if supplied data was identified as status data
void MainWindow::parserStatusReceived(Parser::parser_rx_status_t rx_status, int packet_time_ms)
{
    //Append data to graph
    qreal set_position_percent = (rx_status.pid_setpoint - ui->spinBox_Potentiometer_Start_Margin->value()) / ((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f);
    qreal act_position_percent = (rx_status.potentiometer_position - ui->spinBox_Potentiometer_Start_Margin->value()) / ((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f);
    qreal motor_power_percent = (float)rx_status.motor_power / 10.0f;
    if(!ui->pushButton_Device_Pause_Resume->isChecked()){
        graph->append(set_position_percent, act_position_percent, motor_power_percent, (qreal)packet_time_ms / (qreal)1000.0);
    }
    //Show PID is running or not
    ui->checkBox_PID_Running->setChecked(rx_status.pid_running);
    //Display potentiometer position
    ui->dial_Potentiometer->setValue((uint16_t )rx_status.potentiometer_position);
    //Update text on potentiometer value label
    ui->label_Potentiometer_Current_Step->setText(QString("Current step: ").append(QString::number((uint16_t)rx_status.potentiometer_position)));

    //Revert/Store values in flash if requested so
    if(keep_revert_flash){
        usb.send(parser.configRequest(keep_revert_flash));
        keep_revert_flash = Parser::CMD_SETTINGS_KEEP;
        return;
    }

    //Assemble configuration for status request
    Parser::parser_config_t config = {
        .potentiometer_min = (uint16_t)ui->spinBox_Potentiometer_Start_Margin->value(),
        .potentiometer_max = (uint16_t)ui->spinBox_Potentiometer_End_Margin->value(),
        .motor_max_power = (uint8_t)ui->horizontalSlider_Motor_Max_Power->value(),
        .motor_invert = ui->checkBox_Motor_Invert->isChecked(),
        .signal_ignore = ui->checkBox_Signal_Ignore_On_USB->isChecked(),
        .signal_min = (float)ui->doubleSpinBox_Signal_Min->value(),
        .signal_max = (float)ui->doubleSpinBox_Signal_Max->value(),
        .signal_timeout = (uint32_t)ui->spinBox_Signal_Timeout->value(),
        .led_mode = 0,
        .pid_on_e_m = ui->checkBox_PID_P_On_M->isChecked(),
        .pid_kp_1 = (float)ui->doubleSpinBox_PID_P_1->value(),
        .pid_ki_1 = (float)ui->doubleSpinBox_PID_I_1->value(),
        .pid_kd_1 = (float)ui->doubleSpinBox_PID_D_1->value(),
        .pid_kp_2 = (float)ui->doubleSpinBox_PID_P_2->value(),
        .pid_ki_2 = (float)ui->doubleSpinBox_PID_I_2->value(),
        .pid_kd_2 = (float)ui->doubleSpinBox_PID_D_2->value(),
        .pid_kp_3 = (float)ui->doubleSpinBox_PID_P_3->value(),
        .pid_ki_3 = (float)ui->doubleSpinBox_PID_I_3->value(),
        .pid_kd_3 = (float)ui->doubleSpinBox_PID_D_3->value(),
        .pid_split_steps_1 = (uint16_t)(((float)ui->doubleSpinBox_PID_Split_1->value() * (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f)) + (float)ui->spinBox_Potentiometer_Start_Margin->value()),
        .pid_split_steps_2 = (uint16_t)(((float)ui->doubleSpinBox_PID_Split_2->value() * (((float)ui->spinBox_Potentiometer_End_Margin->value() - (float)ui->spinBox_Potentiometer_Start_Margin->value()) / 100.0f)) + (float)ui->spinBox_Potentiometer_Start_Margin->value()),
        .pid_sampling_ms = (uint8_t)((int)1000 / ui->spinBox_PID_Sampling_Hz->value())
    };

    if(ui->radioButton_LED_Power->isChecked()){
        config.led_mode = 1;
    }else if(ui->radioButton_LED_Signal->isChecked()){
        config.led_mode = 2;
    }else if(ui->radioButton_LED_Position_Change->isChecked()){
        config.led_mode = 3;
    }

    //Assemble transmit status for status request
    Parser::parser_tx_status_t tx_status{
        .pid_setpoint = 0,
        .usb_rq_stat_phold = false,
        .usb_rq_stat_motor = 0
    };

    if(ui->pushButton_Signal_Test_Hold->isChecked()){
        tx_status.pid_setpoint = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_Signal_Test_Position->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
    }else if(ui->pushButton_PID_Test_Oscillate->isChecked()){
        if(!pid_test_oscillate_edge){
            tx_status.pid_setpoint = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_PID_Test_Pos_A->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
        }else{
            tx_status.pid_setpoint = (((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f) * ui->doubleSpinBox_PID_Test_Pos_B->value()) + ui->spinBox_Potentiometer_Start_Margin->value();
        }
    }

    tx_status.usb_rq_stat_phold = ui->pushButton_Signal_Test_Hold->isChecked() || ui->pushButton_PID_Test_Oscillate->isChecked();

    if(ui->pushButton_Motor_Test_Backward->isDown()){
        tx_status.usb_rq_stat_motor = 1;
    }else if(ui->pushButton_Motor_Test_Forward->isDown()){
        tx_status.usb_rq_stat_motor = 2;
    }

    //Request status again
    usb.send(parser.statusRequest(&config, &tx_status));

    //Enable UI
    if(ui->pushButton_Signal_Test_Hold->isChecked()){
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_SIGNAL_TEST_HOLD]);
    }
    else if(!ui->checkBox_Signal_Ignore_On_USB->isChecked()){
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_SIGNAL_IGNORE_DISABLED]);
    }
    else if(ui->pushButton_PID_Test_Oscillate->isChecked()){
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_PID_TEST_OSCILLATE]);
    }
    else{
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING]);
    }
}

//Timer interrupt for PID oscillation test
void MainWindow::pidTestOscTimOvf()
{
    pid_test_oscillate_edge = 1 - pid_test_oscillate_edge;
}

//Enables disables UI elements according to provided "bool" array
void MainWindow::uiEnable(ui_enable_t *enable)
{
    ui->groupBox_Potentiometer->setEnabled(enable->group_pot);
    ui->groupBox_Motor->setEnabled(enable->group_motor);
    ui->groupBox_Signal->setEnabled(enable->group_signal);
    ui->groupBox_LED->setEnabled(enable->group_led);
    ui->groupBox_PID->setEnabled(enable->group_pid);
    ui->checkBox_Signal_Ignore_On_USB->setEnabled(enable->checkbox_signal_ignore);
    ui->pushButton_Device_Pause_Resume->setEnabled(enable->button_pause_resume);
    ui->actionStoreToFlash->setEnabled(enable->action_update_flash);
    ui->actionRevertFromFlash->setEnabled(enable->action_revert_flash);
    ui->actionLoadConfiguration->setEnabled(enable->action_load_configuration);
    ui->actionSaveConfiguration->setEnabled(enable->action_save_configuration);
    ui->pushButton_Signal_Test_Hold->setEnabled(enable->button_signal_hold);
    ui->pushButton_PID_Test_Oscillate->setEnabled(enable->button_pid_test_oscillate);
    ui->pushButton_Motor_Test_Backward->setEnabled(enable->button_motor_test_backward);
    ui->pushButton_Motor_Test_Forward->setEnabled(enable->button_motor_test_forward);
}

/*
    UI Interrupts
*/

//Graph zoom interrupt
void MainWindow::on_actionGraphZoom_triggered()
{
    ui->doubleSpinBox_Device_Vertical_Start->setMaximum(ui->doubleSpinBox_Device_Vertical_End->value() - 1.0f);
    ui->doubleSpinBox_Device_Horizontal_Start->setMaximum(ui->doubleSpinBox_Device_Horizontal_End->value() - 1.0f);
    ui->doubleSpinBox_Device_Vertical_End->setMinimum(ui->doubleSpinBox_Device_Vertical_Start->value() + 1.0f);
    ui->doubleSpinBox_Device_Horizontal_End->setMinimum(ui->doubleSpinBox_Device_Horizontal_Start->value() + 1.0f);

    graph->zoom((qreal)ui->doubleSpinBox_Device_Vertical_Start->value(), (qreal)ui->doubleSpinBox_Device_Vertical_End->value(), (qreal)ui->doubleSpinBox_Device_Horizontal_Start->value(), (qreal)ui->doubleSpinBox_Device_Horizontal_End->value());
}
//Graph buffer clip checkbox interrupt
void MainWindow::on_actionGraphBufferClip_triggered()
{
    on_actionGraphZoom_triggered();
    graph->clipBufferEnd(ui->checkBox_Device_Buffer_End->isChecked());
}

//Potentiometer "set start" button click interrupt
void MainWindow::on_actionPotentiometerSetStart_triggered()
{
    if(ui->dial_Potentiometer->value() > (ui->spinBox_Potentiometer_End_Margin->value() - 128)){
        ui->spinBox_Potentiometer_Start_Margin->setValue(ui->spinBox_Potentiometer_End_Margin->value() - 128);
    }else{
        ui->spinBox_Potentiometer_Start_Margin->setValue(ui->dial_Potentiometer->value());
    }
}
//Potentiometer "set end" button click interrupt
void MainWindow::on_actionPotentiometerSetEnd_triggered()
{
    if(ui->dial_Potentiometer->value() < (ui->spinBox_Potentiometer_Start_Margin->value() + 128)){
        ui->spinBox_Potentiometer_End_Margin->setValue(ui->spinBox_Potentiometer_Start_Margin->value() + 128);
    }else{
        ui->spinBox_Potentiometer_End_Margin->setValue(ui->dial_Potentiometer->value());
    }
}
//Potentiometer start margin spinbox value change interrupt
void MainWindow::on_actionPotentiometerStartMarginSpinbox_triggered()
{
    if(ui->spinBox_Potentiometer_End_Margin->value() < ui->spinBox_Potentiometer_Start_Margin->value() + 128){
        ui->spinBox_Potentiometer_End_Margin->setValue(ui->spinBox_Potentiometer_Start_Margin->value() + 128);
    }
}
//Potentiometer end margin spinbox value change interrupt
void MainWindow::on_actionPotentiometerEndMarginSpinbox_triggered()
{
    if(ui->spinBox_Potentiometer_Start_Margin->value() > ui->spinBox_Potentiometer_End_Margin->value() - 128){
        ui->spinBox_Potentiometer_Start_Margin->setValue(ui->spinBox_Potentiometer_End_Margin->value() - 128);
    }
}

//Motor max power slider value change interrupt
void MainWindow::on_actionMotorMaxPowerChanged_triggered()
{
    ui->label_Motor_Max_Power_Val->setText(QString::number(ui->horizontalSlider_Motor_Max_Power->value()).append("%"));
}

//PID oscillate test button click
void MainWindow::on_actionPIDTestOscillate_triggered()
{
    if(ui->pushButton_PID_Test_Oscillate->isChecked()){
        pid_test_osc_timer.start(ui->spinBox_PID_Test_Time->value() * 1000);
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_PID_TEST_OSCILLATE]);
    }else{
        pid_test_osc_timer.stop();
        pid_test_oscillate_edge = 0;
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING]);
    }
}

//Signal position hold button click
void MainWindow::on_actionSignalTestHold_triggered()
{
    if(ui->pushButton_Signal_Test_Hold->isChecked()){
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_SIGNAL_TEST_HOLD]);
    }else{
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING]);
    }
}
//Signal min and max spinboxes value change interrupt
void MainWindow::on_actionSignalMinMaxChanged_triggered()
{
    ui->doubleSpinBox_Signal_Min->setMaximum(ui->doubleSpinBox_Signal_Max->value() - SIGNAL_POSITION_MIN_LENGTH);
    ui->doubleSpinBox_Signal_Max->setMinimum(ui->doubleSpinBox_Signal_Min->value() + SIGNAL_POSITION_MIN_LENGTH);
}
//Signal "ignore on usb" checkbox check/uncheck interrupt
void MainWindow::on_actionSignalIgnoreOnUSB_triggered()
{
    if(ui->checkBox_Signal_Ignore_On_USB->isChecked()){
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING]);
    }else{
        ui->pushButton_Signal_Test_Hold->setChecked(false);
        ui->pushButton_PID_Test_Oscillate->setChecked(false);
        uiEnable(&ui_enable[UI_ENABLE_STATE_RUNNING_SIGNAL_IGNORE_DISABLED]);
    }
}

//File menu - Load
void MainWindow::on_actionLoadConfiguration_triggered()
{
    File::file_data_t data;

    if(File().get(&data))
    {
        ui->spinBox_Potentiometer_Start_Margin->setValue(data.pot_start);
        ui->spinBox_Potentiometer_End_Margin->setValue(data.pot_end);
        ui->checkBox_Motor_Invert->setChecked(data.motor_inv);
        ui->horizontalSlider_Motor_Max_Power->setValue(data.motor_max);
        ui->checkBox_Signal_Ignore_On_USB->setChecked(data.signal_usb_ignore);
        ui->doubleSpinBox_Signal_Min->setValue(data.signal_min);
        ui->doubleSpinBox_Signal_Max->setValue(data.signal_max);
        ui->spinBox_Signal_Timeout->setValue(data.signal_timeout);
        switch(data.led_mode){
            case 0:
                ui->radioButton_LED_Off->setChecked(true);
                break;
            case 1:
                ui->radioButton_LED_Power->setChecked(true);
                break;
            case 2:
                ui->radioButton_LED_Signal->setChecked(true);
                break;
            case 3:
                ui->radioButton_LED_Position_Change->setChecked(true);
                break;
        }
        ui->checkBox_PID_P_On_M->setChecked(data.pid_p_on_m);
        ui->doubleSpinBox_PID_P_1->setValue(data.pid_p1);
        ui->doubleSpinBox_PID_I_1->setValue(data.pid_i1);
        ui->doubleSpinBox_PID_D_1->setValue(data.pid_d1);
        ui->doubleSpinBox_PID_P_2->setValue(data.pid_p2);
        ui->doubleSpinBox_PID_I_2->setValue(data.pid_i2);
        ui->doubleSpinBox_PID_D_2->setValue(data.pid_d2);
        ui->doubleSpinBox_PID_P_3->setValue(data.pid_p3);
        ui->doubleSpinBox_PID_I_3->setValue(data.pid_i3);
        ui->doubleSpinBox_PID_D_3->setValue(data.pid_d3);
        ui->doubleSpinBox_PID_Split_1->setValue(data.pid_split_1);
        ui->doubleSpinBox_PID_Split_2->setValue(data.pid_split_2);
        ui->spinBox_PID_Sampling_Hz->setValue(data.pid_hz);

        //Update texts
        ui->label_Motor_Max_Power_Val->setText(QString::number(ui->horizontalSlider_Motor_Max_Power->value()) + "%");
        ui->label_Potentiometer_Current_Step->setText("Current step: " + QString::number(ui->dial_Potentiometer->value()));
    }
}
//File menu - Save
void MainWindow::on_actionSaveConfiguration_triggered()
{
    File::file_data_t data;

    data.pot_start = ui->spinBox_Potentiometer_Start_Margin->value();
    data.pot_end = ui->spinBox_Potentiometer_End_Margin->value();
    data.motor_inv = ui->checkBox_Motor_Invert->isChecked();
    data.motor_max = ui->horizontalSlider_Motor_Max_Power->value();
    data.signal_usb_ignore = ui->checkBox_Signal_Ignore_On_USB->isChecked();
    data.signal_min = ui->doubleSpinBox_Signal_Min->value();
    data.signal_max = ui->doubleSpinBox_Signal_Max->value();
    data.signal_timeout = ui->spinBox_Signal_Timeout->value();
    if(ui->radioButton_LED_Off->isChecked()){
        data.led_mode = 0;
    }else if(ui->radioButton_LED_Power->isChecked()){
        data.led_mode = 1;
    }else if(ui->radioButton_LED_Signal->isChecked()){
        data.led_mode = 2;
    }else if(ui->radioButton_LED_Position_Change->isChecked()){
        data.led_mode = 3;
    }
    data.pid_p_on_m = ui->checkBox_PID_P_On_M->isChecked();
    data.pid_p1 = ui->doubleSpinBox_PID_P_1->value();
    data.pid_i1 = ui->doubleSpinBox_PID_I_1->value();
    data.pid_d1 = ui->doubleSpinBox_PID_D_1->value();
    data.pid_p2 = ui->doubleSpinBox_PID_P_2->value();
    data.pid_i2 = ui->doubleSpinBox_PID_I_2->value();
    data.pid_d2 = ui->doubleSpinBox_PID_D_2->value();
    data.pid_p3 = ui->doubleSpinBox_PID_P_3->value();
    data.pid_i3 = ui->doubleSpinBox_PID_I_3->value();
    data.pid_d3 = ui->doubleSpinBox_PID_D_3->value();
    data.pid_split_1 = ui->doubleSpinBox_PID_Split_1->value();
    data.pid_split_2 = ui->doubleSpinBox_PID_Split_2->value();
    data.pid_hz = ui->spinBox_PID_Sampling_Hz->value();

    File().save(&data);
}

//Flash menu - Revert
void MainWindow::on_actionRevertFromFlash_triggered(){
    uiEnable(&ui_enable[UI_ENABLE_STATE_ALL_DISABLED]);
    keep_revert_flash = Parser::CMD_SETTINGS_REVERT;
}
//Flash menu - Store
void MainWindow::on_actionStoreToFlash_triggered(){
    uiEnable(&ui_enable[UI_ENABLE_STATE_ALL_DISABLED]);
    keep_revert_flash = Parser::CMD_SETTINGS_FLASH;
}

//Help menu - About
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutbox(this);
    aboutbox.setIconPixmap(QPixmap(":/Resources/about.svg"));
    aboutbox.setWindowTitle("About");
    aboutbox.setText("Servo Dashboard - " + VERSION +
                     "\n\n"
                     "This software is a part of the\n"
                     "open source servo motor project.\n"
                     "\n"
                     "https://rocket-scientist.me");
    aboutbox.exec();
}
